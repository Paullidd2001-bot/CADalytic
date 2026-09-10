#pragma once

#include <cstdint>
#include <vector>

#include "../Command.h"
#include "../../core/constraint/Constraint.h"
#include "../../core/sketch/Sketch.h"

namespace cadalytic {

// Adds a constraint (with its geometry bindings) to a sketch; undo removes it.
class AddConstraintCommand : public Command
{
public:
    AddConstraintCommand(Sketch& sketch,
                         ConstraintType type,
                         double value,
                         std::vector<std::uint64_t> pointIds,
                         std::vector<std::uint64_t> lineIds = {})
        : Command("Add constraint"),
          m_sketch(sketch),
          m_type(type),
          m_value(value),
          m_pointIds(std::move(pointIds)),
          m_lineIds(std::move(lineIds))
    {
    }

    std::uint64_t constraintId() const { return m_constraintId; }

    void execute() override
    {
        if (m_constraintId != 0) {
            return;
        }
        const Constraint& constraint =
            m_sketch.addConstraint(m_type, m_value, m_pointIds, m_lineIds);
        m_constraintId = constraint.id();
    }

    void undo() override
    {
        if (m_constraintId == 0) {
            setError("AddConstraint: nothing to undo");
            return;
        }
        m_sketch.removeConstraint(m_constraintId);
        m_constraintId = 0;
    }

    void redo() override { execute(); }

private:
    Sketch& m_sketch;
    ConstraintType m_type;
    double m_value;
    std::vector<std::uint64_t> m_pointIds;
    std::vector<std::uint64_t> m_lineIds;
    std::uint64_t m_constraintId = 0;
};

} // namespace cadalytic