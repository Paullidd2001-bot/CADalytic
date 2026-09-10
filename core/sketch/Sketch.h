#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "../constraint/Constraint.h"
#include "../feature/Feature.h"
#include "SketchLine.h"
#include "SketchPoint.h"

namespace cadalytic {

class Sketch : public Feature
{
public:
    Sketch(std::uint64_t id, std::string name);

    // --- Geometry -----------------------------------------------------------
    SketchPoint& addPoint(double x, double y, bool fixed = false);
    SketchLine& addLine(std::uint64_t startPointId, std::uint64_t endPointId);
    const std::vector<std::unique_ptr<SketchPoint>>& points() const
    {
        return m_points;
    }
    const std::vector<std::unique_ptr<SketchLine>>& lines() const
    {
        return m_lines;
    }

    SketchPoint* findPoint(std::uint64_t pointId);
    const SketchPoint* findPoint(std::uint64_t pointId) const;
    SketchLine* findLine(std::uint64_t lineId);
    Constraint* findConstraint(std::uint64_t constraintId);

    void clearGeometry();

    // Removal cascades: removing a point removes lines that touch it and
    // constraints bound to it; removing a line removes constraints bound to
    // it. The sketch is marked dirty so the next rebuild picks the change up.
    bool removePoint(std::uint64_t pointId);
    bool removeLine(std::uint64_t lineId);
    bool removeConstraint(std::uint64_t constraintId);

    // --- Constraints --------------------------------------------------------
    Constraint& addConstraint(ConstraintType type, double value = 0.0);
    Constraint& addConstraint(ConstraintType type,
                              double value,
                              std::vector<std::uint64_t> pointIds,
                              std::vector<std::uint64_t> lineIds = {});
    const std::vector<std::unique_ptr<Constraint>>& constraints() const
    {
        return m_constraints;
    }

private:
    std::uint64_t m_nextPointId = 1;
    std::uint64_t m_nextLineId = 1;
    std::uint64_t m_nextConstraintId = 1;
    std::vector<std::unique_ptr<SketchPoint>> m_points;
    std::vector<std::unique_ptr<SketchLine>> m_lines;
    std::vector<std::unique_ptr<Constraint>> m_constraints;
};

} // namespace cadalytic
