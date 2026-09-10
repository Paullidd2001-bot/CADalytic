#pragma once

#include <cstdint>
#include <string>

namespace cadalytic {

// Every kind of change the UI and tooling care about. Commands publish the
// CommandExecuted/UndoPerformed/RedoPerformed events; future phases extend
// this as the document model, rebuild engine and CAM tools wire in.
enum class EventType
{
    DocumentChanged,
    PartAdded,
    PartRemoved,
    FeatureAdded,
    FeatureRemoved,
    FeatureRebuilt,
    SketchChanged,
    ConstraintChanged,
    SelectionChanged,
    CommandExecuted,
    UndoPerformed,
    RedoPerformed,
    RebuildCompleted
};

// A single notification. sourceId/targetId carry context (e.g. the feature id
// for FeatureRebuilt); detail is a free-form human-readable summary.
struct Event
{
    EventType type;
    std::uint64_t sourceId = 0;
    std::uint64_t targetId = 0;
    std::string detail;
};

} // namespace cadalytic