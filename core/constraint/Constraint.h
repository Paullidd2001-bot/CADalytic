#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace cadalytic {

enum class ConstraintType
{
    Coincident,
    Horizontal,
    Vertical,
    Distance,
    Angle
};

class Constraint
{
public:
    Constraint(std::uint64_t id, ConstraintType type, double value = 0.0);

    std::uint64_t id() const { return m_id; }
    ConstraintType type() const { return m_type; }
    void setType(ConstraintType type) { m_type = type; }

    double value() const { return m_value; }
    void setValue(double value) { m_value = value; }

    // Geometry bindings. A constraint applies to the points (and optionally
    // lines) it is bound to; the sketch solver interprets them by type.
    void bindPoint(std::uint64_t pointId);
    void bindLine(std::uint64_t lineId);
    const std::vector<std::uint64_t>& pointIds() const { return m_pointIds; }
    const std::vector<std::uint64_t>& lineIds() const { return m_lineIds; }
    std::size_t pointCount() const { return m_pointIds.size(); }
    std::size_t lineCount() const { return m_lineIds.size(); }

private:
    std::uint64_t m_id;
    ConstraintType m_type;
    double m_value;
    std::vector<std::uint64_t> m_pointIds;
    std::vector<std::uint64_t> m_lineIds;
};

} // namespace cadalytic
