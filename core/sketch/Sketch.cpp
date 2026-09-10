#include "Sketch.h"

#include <algorithm>
#include <cstddef>
#include <utility>

namespace cadalytic {

Sketch::Sketch(std::uint64_t id, std::string name)
    : Feature(id, std::move(name), FeatureType::Sketch)
{
}

SketchPoint& Sketch::addPoint(double x, double y, bool fixed)
{
    auto point = std::make_unique<SketchPoint>(m_nextPointId++, x, y, fixed);
    auto* ptr = point.get();
    m_points.push_back(std::move(point));
    markDirty();
    return *ptr;
}

SketchLine& Sketch::addLine(std::uint64_t startPointId, std::uint64_t endPointId)
{
    auto line = std::make_unique<SketchLine>(m_nextLineId++, startPointId, endPointId);
    auto* ptr = line.get();
    m_lines.push_back(std::move(line));
    markDirty();
    return *ptr;
}

SketchPoint* Sketch::findPoint(std::uint64_t pointId)
{
    const auto it = std::find_if(
        m_points.begin(), m_points.end(),
        [pointId](const std::unique_ptr<SketchPoint>& point) {
            return point->id() == pointId;
        });
    return it == m_points.end() ? nullptr : it->get();
}

const SketchPoint* Sketch::findPoint(std::uint64_t pointId) const
{
    const auto it = std::find_if(
        m_points.begin(), m_points.end(),
        [pointId](const std::unique_ptr<SketchPoint>& point) {
            return point->id() == pointId;
        });
    return it == m_points.end() ? nullptr : it->get();
}

SketchLine* Sketch::findLine(std::uint64_t lineId)
{
    const auto it = std::find_if(
        m_lines.begin(), m_lines.end(),
        [lineId](const std::unique_ptr<SketchLine>& line) {
            return line->id() == lineId;
        });
    return it == m_lines.end() ? nullptr : it->get();
}

Constraint* Sketch::findConstraint(std::uint64_t constraintId)
{
    const auto it = std::find_if(
        m_constraints.begin(), m_constraints.end(),
        [constraintId](const std::unique_ptr<Constraint>& constraint) {
            return constraint->id() == constraintId;
        });
    return it == m_constraints.end() ? nullptr : it->get();
}

void Sketch::clearGeometry()
{
    m_points.clear();
    m_lines.clear();
    m_constraints.clear();
    markDirty();
}

bool Sketch::removeConstraint(std::uint64_t constraintId)
{
    const auto it = std::find_if(
        m_constraints.begin(), m_constraints.end(),
        [constraintId](const std::unique_ptr<Constraint>& constraint) {
            return constraint->id() == constraintId;
        });
    if (it == m_constraints.end()) {
        return false;
    }
    m_constraints.erase(it);
    markDirty();
    return true;
}

bool Sketch::removeLine(std::uint64_t lineId)
{
    const auto lineIt = std::find_if(
        m_lines.begin(), m_lines.end(),
        [lineId](const std::unique_ptr<SketchLine>& line) {
            return line->id() == lineId;
        });
    if (lineIt == m_lines.end()) {
        return false;
    }
    m_lines.erase(lineIt);

    for (std::size_t i = 0; i < m_constraints.size();) {
        const auto& bound = m_constraints[i]->lineIds();
        if (std::find(bound.begin(), bound.end(), lineId) != bound.end()) {
            m_constraints.erase(m_constraints.begin() + static_cast<std::ptrdiff_t>(i));
        } else {
            ++i;
        }
    }
    markDirty();
    return true;
}

bool Sketch::removePoint(std::uint64_t pointId)
{
    const auto pointIt = std::find_if(
        m_points.begin(), m_points.end(),
        [pointId](const std::unique_ptr<SketchPoint>& point) {
            return point->id() == pointId;
        });
    if (pointIt == m_points.end()) {
        return false;
    }
    m_points.erase(pointIt);

    for (std::size_t i = 0; i < m_lines.size();) {
        if (m_lines[i]->touches(pointId)) {
            m_lines.erase(m_lines.begin() + static_cast<std::ptrdiff_t>(i));
        } else {
            ++i;
        }
    }

    for (std::size_t i = 0; i < m_constraints.size();) {
        const auto& bound = m_constraints[i]->pointIds();
        if (std::find(bound.begin(), bound.end(), pointId) != bound.end()) {
            m_constraints.erase(m_constraints.begin() + static_cast<std::ptrdiff_t>(i));
        } else {
            ++i;
        }
    }
    markDirty();
    return true;
}
Constraint& Sketch::addConstraint(ConstraintType type, double value)
{
    return addConstraint(type, value, {}, {});
}

Constraint& Sketch::addConstraint(ConstraintType type,
                                  double value,
                                  std::vector<std::uint64_t> pointIds,
                                  std::vector<std::uint64_t> lineIds)
{
    auto constraint = std::make_unique<Constraint>(m_nextConstraintId++, type, value);
    for (const auto pointId : pointIds) {
        constraint->bindPoint(pointId);
    }
    for (const auto lineId : lineIds) {
        constraint->bindLine(lineId);
    }
    auto* ptr = constraint.get();
    m_constraints.push_back(std::move(constraint));
    markDirty();
    return *ptr;
}

} // namespace cadalytic
