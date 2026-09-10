#include "DOFAnalyzer.h"
#include "SketchConstraints.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cadalytic {

namespace {

class UnionFind
{
public:
    std::size_t find(std::size_t x)
    {
        if (m_parent[x] != x) {
            m_parent[x] = find(m_parent[x]);
        }
        return m_parent[x];
    }

    void unite(std::size_t a, std::size_t b)
    {
        const std::size_t ra = find(a);
        const std::size_t rb = find(b);
        if (ra == rb) {
            return;
        }
        m_parent[rb] = ra;
    }

    // Ensures slot x exists; entries are their own root by construction.
    void add(std::size_t x)
    {
        if (m_parent.size() <= x) {
            for (std::size_t i = m_parent.size(); i <= x; ++i) {
                m_parent.push_back(i);
            }
        }
    }

    std::size_t distinctRoots()
    {
        std::size_t count = 0;
        std::vector<bool> seen(m_parent.size(), false);
        for (std::size_t i = 0; i < m_parent.size(); ++i) {
            const std::size_t root = find(i);
            if (!seen[root]) {
                ++count;
                seen[root] = true;
            }
        }
        return count;
    }

private:
    std::vector<std::size_t> m_parent;
};

} // namespace

DOFReport DOFAnalyzer::analyze(const Sketch& sketch) const
{
    DOFReport report;
    report.pointCount = sketch.points().size();
    report.fixedPointCount = 0;
    report.parameterCount = 0;

    std::unordered_map<std::uint64_t, std::size_t> pointSlot;
    pointSlot.reserve(sketch.points().size());
    for (std::size_t i = 0; i < sketch.points().size(); ++i) {
        pointSlot[sketch.points()[i]->id()] = i;
        if (sketch.points()[i]->fixed()) {
            ++report.fixedPointCount;
        } else {
            report.parameterCount += 2;
        }
    }
    std::unordered_map<std::uint64_t, std::size_t> lineSlot;
    lineSlot.reserve(sketch.lines().size());
    for (std::size_t i = 0; i < sketch.lines().size(); ++i) {
        lineSlot[sketch.lines()[i]->id()] = i;
    }

    UnionFind components;
    if (sketch.points().size() > 0) {
        components.add(sketch.points().size() - 1);
    }

    for (const auto& constraint : sketch.constraints()) {
        // Validate bindings (mirrors the solver's own validation).
        bool valid = true;
        switch (constraint->type()) {
        case ConstraintType::Coincident:
        case ConstraintType::Horizontal:
        case ConstraintType::Vertical:
        case ConstraintType::Distance:
            valid = constraint->pointCount() == 2 && constraint->lineCount() == 0;
            break;
        case ConstraintType::Angle:
            valid = constraint->lineCount() == 2 && constraint->pointCount() == 0;
            break;
        }
        if (!valid) {
            report.malformed = true;
            report.detail = "Constraint " + std::to_string(constraint->id()) +
                            " has an invalid binding for its type";
            return report;
        }
        for (const auto pointId : constraint->pointIds()) {
            const auto it = pointSlot.find(pointId);
            if (it == pointSlot.end()) {
                report.malformed = true;
                report.detail = "Constraint " + std::to_string(constraint->id()) +
                                " references missing point " + std::to_string(pointId);
                return report;
            }
            components.add(it->second);
        }
        for (const auto lineId : constraint->lineIds()) {
            if (lineSlot.find(lineId) == lineSlot.end()) {
                report.malformed = true;
                report.detail = "Constraint " + std::to_string(constraint->id()) +
                                " references missing line " + std::to_string(lineId);
                return report;
            }
        }

        // The graph edge: all bound points of a constraint are connected.
        report.equationCount += constraintEquationCount(constraint->type());
        if (constraint->pointCount() == 2) {
            components.unite(pointSlot.at(constraint->pointIds()[0]),
                             pointSlot.at(constraint->pointIds()[1]));
        }
    }

    // Lines are rigid links between their endpoints, so they connect points too.
    for (const auto& line : sketch.lines()) {
        components.unite(pointSlot.at(line->startPointId()),
                         pointSlot.at(line->endPointId()));
    }

    report.componentCount = sketch.points().empty() ? 0 : components.distinctRoots();

    if (report.malformed) {
        return report;
    }
    if (report.equationCount > report.parameterCount) {
        report.overconstrained = true;
        report.degreesOfFreedom = 0;
        return report;
    }
    report.degreesOfFreedom = report.parameterCount - report.equationCount;
    report.underconstrained = report.degreesOfFreedom > 0;
    report.fullyConstrained = !report.underconstrained;
    return report;
}

} // namespace cadalytic