#include "Constraint.h"

namespace cadalytic {

Constraint::Constraint(std::uint64_t id, ConstraintType type, double value)
    : m_id(id),
      m_type(type),
      m_value(value)
{
}

} // namespace cadalytic
