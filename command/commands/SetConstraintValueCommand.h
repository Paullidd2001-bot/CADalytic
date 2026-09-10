#pragma once

#include <cstdint>

#include "../Command.h"
#include "../../core/sketch/Sketch.h"

namespace cadalytic {

// Changes the value of a sketch constraint. Consecutive value changes on the
// same constraint merge into a single undo step.
class SetConstraintValueCommand : public Command
{
public:
    SetConstraintValueCommand(Sketch& sketch,
                              std::uint64_t constraintId,
                              double newValue)
        : Command("Set constraint value"),
          m_sketch(sketch),
          m_constraintId(constraintId),
          m_newValue(newValue)
    {
        const Constraint* constraint = sketch.findConstraint(constraintId);
        m_oldValue = constraint != nullptr ? constraint->value() : 0.0;
    }

    void execute() override
    {
        if (Constraint* constraint = m_sketch.findConstraint(m_constraintId)) {
            constraint->setValue(m_newValue);
        } else {
            setError("SetConstraintValue: constraint not found");
        }
    }

    void undo() override
    {
        if (Constraint* constraint = m_sketch.findConstraint(m_constraintId)) {
            constraint->setValue(m_oldValue);
        } else {
            setError("SetConstraintValue: constraint not found");
        }
    }

    void redo() override { execute(); }

    std::uint64_t mergeKey() const override
    {
        return m_sketch.id() * 1000000ULL + m_constraintId;
    }

    bool mergeable() const override { return true; }

    void merge(const Command& other) override
    {
        const auto* otherCommand =
            dynamic_cast<const SetConstraintValueCommand*>(&other);
        if (otherCommand != nullptr) {
            m_newValue = otherCommand->m_newValue;
        }
    }

    std::uint64_t constraintId() const { return m_constraintId; }
    double oldValue() const { return m_oldValue; }

private:
    Sketch& m_sketch;
    std::uint64_t m_constraintId;
    double m_oldValue;
    double m_newValue;
};

} // namespace cadalytic