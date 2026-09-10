#include "SketchPoint.h"

namespace cadalytic {

SketchPoint::SketchPoint(std::uint64_t id, double x, double y, bool fixed)
    : m_id(id),
      m_x(x),
      m_y(y),
      m_fixed(fixed)
{
}

} // namespace cadalytic