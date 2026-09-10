#pragma once

#include <cstdint>

namespace cadalytic {

// A line segment in a sketch. The segment is defined by two sketch points;
// its geometry is always derived from the current point positions.
class SketchLine
{
public:
    SketchLine(std::uint64_t id, std::uint64_t startPointId, std::uint64_t endPointId);

    std::uint64_t id() const { return m_id; }
    std::uint64_t startPointId() const { return m_startPointId; }
    std::uint64_t endPointId() const { return m_endPointId; }

    bool touches(std::uint64_t pointId) const
    {
        return m_startPointId == pointId || m_endPointId == pointId;
    }

private:
    std::uint64_t m_id;
    std::uint64_t m_startPointId;
    std::uint64_t m_endPointId;
};

} // namespace cadalytic