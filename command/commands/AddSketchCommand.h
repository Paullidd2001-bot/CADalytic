#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "../Command.h"
#include "../../core/document/Part.h"

namespace cadalytic {

// Adds a sketch feature to a part; undo removes the sketch from the part.
class AddSketchCommand : public Command
{
public:
    AddSketchCommand(Part& part, std::string name)
        : Command("Add sketch"),
          m_part(part),
          m_name(std::move(name))
    {
    }

    std::uint64_t featureId() const { return m_featureId; }

    void execute() override
    {
        if (m_featureId != 0) {
            return;
        }
        const Sketch& sketch = m_part.addSketch(m_name);
        m_featureId = sketch.id();
    }

    void undo() override
    {
        if (m_featureId == 0) {
            setError("AddSketch: nothing to undo");
            return;
        }
        m_part.removeFeature(m_featureId);
        m_featureId = 0;
    }

    void redo() override { execute(); }

private:
    Part& m_part;
    std::string m_name;
    std::uint64_t m_featureId = 0;
};

} // namespace cadalytic