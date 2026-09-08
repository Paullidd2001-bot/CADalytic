#include "Sketch.h"

#include <utility>

namespace cadalytic {

Sketch::Sketch(std::uint64_t id, std::string name)
    : Feature(id, std::move(name), FeatureType::Sketch)
{
}

Constraint& Sketch::addConstraint(ConstraintType type, double value)
{
    auto constraint = std::make_unique<Constraint>(m_nextConstraintId++, type, value);
    m_constraints.push_back(std::move(constraint));
    return *m_constraints.back();
}

} // namespace cadalytic
