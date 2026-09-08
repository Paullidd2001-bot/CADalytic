#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "../constraint/Constraint.h"
#include "../feature/Feature.h"

namespace cadalytic {

class Sketch : public Feature
{
public:
    Sketch(std::uint64_t id, std::string name);

    Constraint& addConstraint(ConstraintType type, double value = 0.0);
    const std::vector<std::unique_ptr<Constraint>>& constraints() const
    {
        return m_constraints;
    }

private:
    std::uint64_t m_nextConstraintId = 1;
    std::vector<std::unique_ptr<Constraint>> m_constraints;
};

} // namespace cadalytic
