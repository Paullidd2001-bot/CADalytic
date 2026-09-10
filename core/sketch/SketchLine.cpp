#include "SketchLine.h"

namespace cadalytic {

SketchLine::SketchLine(std::uint64_t id, std::uint64_t startPointId, std::uint64_t endPointId)
    : m_id(id),
      m_startPointId(startPointId),
      m_endPointId(endPointId)
{
}

} // namespace cadalytic