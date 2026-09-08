#pragma once

#include <cstdint>
#include <string>

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

private:
    std::uint64_t m_id;
    ConstraintType m_type;
    double m_value;
};

} // namespace cadalytic
