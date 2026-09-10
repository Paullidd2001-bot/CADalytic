#pragma once

#include <cstdint>

namespace cadalytic {

// A 2D sketch point. Fixed points do not participate in solving: the solver
// treats their coordinates as constants.
class SketchPoint
{
public:
    SketchPoint(std::uint64_t id, double x, double y, bool fixed = false);

    std::uint64_t id() const { return m_id; }

    double x() const { return m_x; }
    void setX(double x) { m_x = x; }

    double y() const { return m_y; }
    void setY(double y) { m_y = y; }

    void setPosition(double x, double y)
    {
        m_x = x;
        m_y = y;
    }

    bool fixed() const { return m_fixed; }
    void setFixed(bool fixed) { m_fixed = fixed; }

private:
    std::uint64_t m_id;
    double m_x;
    double m_y;
    bool m_fixed;
};

} // namespace cadalytic