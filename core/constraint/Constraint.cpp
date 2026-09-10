#include "Constraint.h"

#include <utility>

namespace cadalytic {

Constraint::Constraint(std::uint64_t id, ConstraintType type, double value)
    : m_id(id),
      m_type(type),
      m_value(value)
{
}

void Constraint::bindPoint(std::uint64_t pointId)
{
    if (std::find(m_pointIds.begin(), m_pointIds.end(), pointId) == m_pointIds.end()) {
        m_pointIds.push_back(pointId);
    }
}

void Constraint::bindLine(std::uint64_t lineId)
{
    if (std::find(m_lineIds.begin(), m_lineIds.end(), lineId) == m_lineIds.end()) {
        m_lineIds.push_back(lineId);
    }
}

} // namespace cadalytic
