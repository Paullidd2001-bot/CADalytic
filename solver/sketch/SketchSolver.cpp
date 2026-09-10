#include "SketchSolver.h"
#include "SketchConstraints.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <unordered_map>

namespace cadalytic {

namespace {

constexpr std::size_t noIndex = std::numeric_limits<std::size_t>::max();

std::size_t maxAbs(const std::vector<double>& values)
{
    if (values.empty()) {
        return noIndex;
    }
    std::size_t best = 0;
    for (std::size_t i = 1; i < values.size(); ++i) {
        if (std::abs(values[i]) > std::abs(values[best])) {
            best = i;
        }
    }
    return std::abs(values[best]) < 1e-14 ? noIndex : best;
}

// Solves A x = b in place by Gaussian elimination with partial pivoting.
// Returns false when the matrix is numerically singular.
bool solveLinear(std::vector<std::vector<double>>& a,
                 std::vector<double>& b,
                 std::vector<double>& x)
{
    const std::size_t n = a.size();
    for (std::size_t k = 0; k < n; ++k) {
        std::size_t pivot = k;
        double best = std::abs(a[k][k]);
        for (std::size_t i = k + 1; i < n; ++i) {
            const double v = std::abs(a[i][k]);
            if (v > best) {
                best = v;
                pivot = i;
            }
        }
        if (best < 1e-12) {
            return false;
        }
        if (pivot != k) {
            std::swap(a[k], a[pivot]);
            std::swap(b[k], b[pivot]);
        }
        for (std::size_t i = k + 1; i < n; ++i) {
            const double f = a[i][k] / a[k][k];
            for (std::size_t j = k + 1; j < n; ++j) {
                a[i][j] -= f * a[k][j];
            }
            b[i] -= f * b[k];
        }
    }
    x.resize(n);
    for (std::size_t i = n; i-- > 0;) {
        double sum = b[i];
        for (std::size_t j = i + 1; j < n; ++j) {
            sum -= a[i][j] * x[j];
        }
        x[i] = sum / a[i][i];
    }
    return true;
}

double wrapToPi(double angle)
{
    return std::atan2(std::sin(angle), std::cos(angle));
}

SketchLine* findLineById(const Sketch& sketch, std::uint64_t lineId)
{
    for (const auto& line : sketch.lines()) {
        if (line->id() == lineId) {
            return line.get();
        }
    }
    return nullptr;
}

} // namespace

SketchSolver::SketchSolver(std::size_t maxIterations, double tolerance)
    : m_maxIterations(maxIterations),
      m_tolerance(tolerance)
{
}

SolveResult SketchSolver::solve(Sketch& sketch) const
{
    SolveResult result{};

    // Parameter vector X: x and y of every non-fixed point, in storage order.
    std::unordered_map<std::uint64_t, std::size_t> pointSlot;
    std::unordered_map<std::uint64_t, std::size_t> lineSlot;
    pointSlot.reserve(sketch.points().size());
    lineSlot.reserve(sketch.lines().size());
    for (std::size_t i = 0; i < sketch.points().size(); ++i) {
        pointSlot[sketch.points()[i]->id()] = i;
    }
    for (std::size_t i = 0; i < sketch.lines().size(); ++i) {
        lineSlot[sketch.lines()[i]->id()] = i;
    }

    std::vector<std::size_t> xParam(sketch.points().size(), noIndex);
    std::vector<std::size_t> yParam(sketch.points().size(), noIndex);
    std::vector<double> x;
    for (std::size_t i = 0; i < sketch.points().size(); ++i) {
        const auto& point = sketch.points()[i];
        if (point->fixed()) {
            continue;
        }
        xParam[i] = x.size();
        x.push_back(point->x());
        yParam[i] = x.size();
        x.push_back(point->y());
    }

    // Validate bindings so the residual builder can trust the model.
    for (const auto& constraint : sketch.constraints()) {
        bool validBinding = true;
        switch (constraint->type()) {
        case ConstraintType::Coincident:
        case ConstraintType::Horizontal:
        case ConstraintType::Vertical:
        case ConstraintType::Distance:
            validBinding = constraint->pointCount() == 2 && constraint->lineCount() == 0;
            break;
        case ConstraintType::Angle:
            validBinding = constraint->lineCount() == 2 && constraint->pointCount() == 0;
            break;
        }
        if (!validBinding) {
            result.error = "Constraint " + std::to_string(constraint->id()) +
                           " has an invalid binding for its type";
            return result;
        }
        for (const auto pointId : constraint->pointIds()) {
            if (pointSlot.find(pointId) == pointSlot.end()) {
                result.error = "Constraint " + std::to_string(constraint->id()) +
                               " references missing point " + std::to_string(pointId);
                return result;
            }
        }
        for (const auto lineId : constraint->lineIds()) {
            if (lineSlot.find(lineId) == lineSlot.end()) {
                result.error = "Constraint " + std::to_string(constraint->id()) +
                               " references missing line " + std::to_string(lineId);
                return result;
            }
        }
    }
// Residual builder: for a parameter snapshot, produce one value per
    // constraint equation. Fixed points always read from the live sketch.
    const auto computeResiduals = [&](const std::vector<double>& xv) -> std::vector<double> {
        const auto coordX = [&](std::uint64_t pointId) -> double {
            const std::size_t slot = pointSlot.at(pointId);
            return xParam[slot] == noIndex ? sketch.points()[slot]->x() : xv[xParam[slot]];
        };
        const auto coordY = [&](std::uint64_t pointId) -> double {
            const std::size_t slot = pointSlot.at(pointId);
            return yParam[slot] == noIndex ? sketch.points()[slot]->y() : xv[yParam[slot]];
        };
        const auto lineAngle = [&](std::uint64_t lineId) -> double {
            const SketchLine* line = findLineById(sketch, lineId);
            const double dx = coordX(line->endPointId()) - coordX(line->startPointId());
            const double dy = coordY(line->endPointId()) - coordY(line->startPointId());
            return std::atan2(dy, dx);
        };

        std::vector<double> out;
        for (const auto& constraint : sketch.constraints()) {
            const auto& points = constraint->pointIds();
            const auto& lines = constraint->lineIds();
            switch (constraint->type()) {
            case ConstraintType::Coincident:
                out.push_back(coordX(points[1]) - coordX(points[0]));
                out.push_back(coordY(points[1]) - coordY(points[0]));
                break;
            case ConstraintType::Horizontal:
                out.push_back(coordY(points[1]) - coordY(points[0]));
                break;
            case ConstraintType::Vertical:
                out.push_back(coordX(points[1]) - coordX(points[0]));
                break;
            case ConstraintType::Distance: {
                const double dx = coordX(points[1]) - coordX(points[0]);
                const double dy = coordY(points[1]) - coordY(points[0]);
                out.push_back(std::hypot(dx, dy) - constraint->value());
                break;
            }
            case ConstraintType::Angle:
                out.push_back(wrapToPi(lineAngle(lines[1]) - lineAngle(lines[0])
                                       - constraint->value()));
                break;
            }
        }
        return out;
    };

    std::vector<double> residuals = computeResiduals(x);
    if (residuals.empty()) {
        result.success = true;
        result.maxResidual = 0.0;
        return result;
    }
    std::size_t worst = maxAbs(residuals);
    if (worst != noIndex && std::abs(residuals[worst]) <= m_tolerance) {
        result.success = true;
        result.maxResidual = std::abs(residuals[worst]);
        return result;
    }

    const std::size_t paramCount = x.size();
    if (paramCount == 0) {
        result.error = "Constraints exist but there are no free parameters to adjust";
        return result;
    }

    for (std::size_t iteration = 0; iteration < m_maxIterations; ++iteration) {
        residuals = computeResiduals(x);
        worst = maxAbs(residuals);
        if (worst == noIndex || std::abs(residuals[worst]) <= m_tolerance) {
            result.success = true;
            result.iterations = iteration;
            result.maxResidual = worst == noIndex ? 0.0 : std::abs(residuals[worst]);
            break;
        }

        // --- Numeric Jacobian by central differences -------------------------
        std::vector<std::vector<double>> jacobian(
            residuals.size(), std::vector<double>(paramCount, 0.0));
        for (std::size_t j = 0; j < paramCount; ++j) {
            const double h = 1e-7 * std::max(1.0, std::abs(x[j]));
            std::vector<double> plus = x;
            std::vector<double> minus = x;
            plus[j] += h;
            minus[j] -= h;
            const std::vector<double> rPlus = computeResiduals(plus);
            const std::vector<double> rMinus = computeResiduals(minus);
            for (std::size_t i = 0; i < residuals.size(); ++i) {
                jacobian[i][j] = (rPlus[i] - rMinus[i]) / (2.0 * h);
            }
        }
// --- Normal equations with ridge regularisation ----------------------
        // The ridge keeps rank-deficient (underconstrained) systems solvable
        // and makes the iteration converge to a least-squares solution.
        std::vector<std::vector<double>> normal(
            paramCount, std::vector<double>(paramCount, 0.0));
        std::vector<double> gradient(paramCount, 0.0);
        double maxDiagonal = 1.0;
        for (std::size_t j = 0; j < paramCount; ++j) {
            for (std::size_t k = j; k < paramCount; ++k) {
                double sum = 0.0;
                for (std::size_t i = 0; i < residuals.size(); ++i) {
                    sum += jacobian[i][j] * jacobian[i][k];
                }
                normal[j][k] = sum;
                normal[k][j] = sum;
                if (j == k) {
                    maxDiagonal = std::max(maxDiagonal, std::abs(sum));
                }
            }
            for (std::size_t i = 0; i < residuals.size(); ++i) {
                gradient[j] -= jacobian[i][j] * residuals[i];
            }
        }
        const double ridge = 1e-9 * maxDiagonal;
        for (std::size_t j = 0; j < paramCount; ++j) {
            normal[j][j] += ridge;
        }

        std::vector<double> step;
        std::vector<std::vector<double>> normalCopy = normal;
        std::vector<double> gradientCopy = gradient;
        if (!solveLinear(normalCopy, gradientCopy, step)) {
            result.error = "Solver hit a singular normal system";
            return result;
        }

        // --- Damped step ------------------------------------------------------
        // Accept the step only when it reduces the squared residual sum;
        // otherwise halve it until it does.
        double squaredResiduals = 0.0;
        for (const double value : residuals) {
            squaredResiduals += value * value;
        }
        bool accepted = false;
        double damping = 1.0;
        for (std::size_t attempt = 0; attempt < 40; ++attempt) {
            std::vector<double> candidate = x;
            for (std::size_t j = 0; j < paramCount; ++j) {
                candidate[j] += damping * step[j];
            }
            const std::vector<double> candidateResiduals = computeResiduals(candidate);
            double candidateSquared = 0.0;
            for (const double value : candidateResiduals) {
                candidateSquared += value * value;
            }
            if (candidateSquared < squaredResiduals) {
                x = candidate;
                accepted = true;
                break;
            }
            damping *= 0.5;
        }
        if (!accepted) {
            result.error = "Solver stalled: no step improved the residuals";
            return result;
        }
    }

    if (!result.success) {
        result.error = "Solver did not converge within the iteration budget";
        return result;
    }

    // Commit the solved positions back to the sketch. Fixed points are never
    // touched; a failure above leaves everything exactly as it was.
    for (std::size_t i = 0; i < sketch.points().size(); ++i) {
        if (xParam[i] != noIndex) {
            sketch.points()[i]->setPosition(x[xParam[i]], x[yParam[i]]);
        }
    }
    return result;
}

} // namespace cadalytic