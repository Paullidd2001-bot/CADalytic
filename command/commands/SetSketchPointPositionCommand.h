#pragma once

#include <cstdint>
#include <string>

#include "../Command.h"
#include "../../core/sketch/Sketch.h"

namespace cadalytic {

// Moves a sketch point to an absolute position. Consecutive commands that
// move the same point merge, so a drag produces a single undo step.
class SetSketchPointPositionCommand : public Command
{
public:
    SetSketchPointPositionCommand(Sketch& sketch,
                                  std::uint64_t pointId,
                                  double x,
                                  double y)
        : Command("Set sketch point position"),
          m_sketch(sketch),
          m_pointId(pointId),
          m_newX(x),
          m_newY(y)
    {
        const SketchPoint* point = sketch.findPoint(pointId);
        m_oldX = point != nullptr ? point->x() : 0.0;
        m_oldY = point != nullptr ? point->y() : 0.0;
    }

    void execute() override
    {
        if (SketchPoint* point = m_sketch.findPoint(m_pointId)) {
            point->setPosition(m_newX, m_newY);
        } else {
            setError("SetSketchPointPosition: point not found");
        }
    }

    void undo() override
    {
        if (SketchPoint* point = m_sketch.findPoint(m_pointId)) {
            point->setPosition(m_oldX, m_oldY);
        } else {
            setError("SetSketchPointPosition: point not found");
        }
    }

    void redo() override { execute(); }

    std::uint64_t mergeKey() const override
    {
        return m_sketch.id() * 1000000ULL + m_pointId;
    }

    bool mergeable() const override { return true; }

    void merge(const Command& other) override
    {
        const auto* otherCommand =
            dynamic_cast<const SetSketchPointPositionCommand*>(&other);
        if (otherCommand != nullptr) {
            m_newX = otherCommand->m_newX;
            m_newY = otherCommand->m_newY;
        }
    }

    std::uint64_t pointId() const { return m_pointId; }
    double oldX() const { return m_oldX; }
    double oldY() const { return m_oldY; }

private:
    Sketch& m_sketch;
    std::uint64_t m_pointId;
    double m_oldX;
    double m_oldY;
    double m_newX;
    double m_newY;
};

} // namespace cadalytic