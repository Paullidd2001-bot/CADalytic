#pragma once

#include <cstdint>

#include "../Command.h"
#include "../../core/sketch/Sketch.h"

namespace cadalytic {

// Adds a line segment between two existing sketch points; undo removes it
// (and cascades to constraints bound to the line).
class AddSketchLineCommand : public Command
{
public:
    AddSketchLineCommand(Sketch& sketch,
                         std::uint64_t startPointId,
                         std::uint64_t endPointId)
        : Command("Add sketch line"),
          m_sketch(sketch),
          m_startPointId(startPointId),
          m_endPointId(endPointId)
    {
    }

    std::uint64_t lineId() const { return m_lineId; }

    void execute() override
    {
        if (m_lineId != 0) {
            return;
        }
        const SketchLine& line = m_sketch.addLine(m_startPointId, m_endPointId);
        m_lineId = line.id();
    }

    void undo() override
    {
        if (m_lineId == 0) {
            setError("AddSketchLine: nothing to undo");
            return;
        }
        m_sketch.removeLine(m_lineId);
        m_lineId = 0;
    }

    void redo() override { execute(); }

private:
    Sketch& m_sketch;
    std::uint64_t m_startPointId;
    std::uint64_t m_endPointId;
    std::uint64_t m_lineId = 0;
};

} // namespace cadalytic