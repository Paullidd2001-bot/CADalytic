#pragma once

#include <cstdint>

#include "../Command.h"
#include "../../core/sketch/Sketch.h"

namespace cadalytic {

// Adds a 2D point to a sketch; undo removes it (and cascades to any lines or
// constraints bound to it).
class AddSketchPointCommand : public Command
{
public:
    AddSketchPointCommand(Sketch& sketch, double x, double y, bool fixed = false)
        : Command("Add sketch point"),
          m_sketch(sketch),
          m_x(x),
          m_y(y),
          m_fixed(fixed)
    {
    }

    std::uint64_t pointId() const { return m_pointId; }

    void execute() override
    {
        if (m_pointId != 0) {
            return;
        }
        const SketchPoint& point = m_sketch.addPoint(m_x, m_y, m_fixed);
        m_pointId = point.id();
    }

    void undo() override
    {
        if (m_pointId == 0) {
            setError("AddSketchPoint: nothing to undo");
            return;
        }
        m_sketch.removePoint(m_pointId);
        m_pointId = 0;
    }

    void redo() override { execute(); }

private:
    Sketch& m_sketch;
    double m_x;
    double m_y;
    bool m_fixed;
    std::uint64_t m_pointId = 0;
};

} // namespace cadalytic