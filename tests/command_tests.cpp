#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "Command.h"
#include "CommandManager.h"
#include "EventBus.h"
#include "EventTypes.h"
#include "commands/AddSketchPointCommand.h"
#include "commands/SetConstraintValueCommand.h"
#include "commands/SetSketchPointPositionCommand.h"
#include "document/Document.h"

namespace {

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

// A command that fails its own execution by recording an error. The
// CommandManager must keep a failing command off the undo stack.
class FailCommand : public cadalytic::Command
{
public:
    FailCommand() : Command("Fail") {}
    void execute() override { setError("intentional failure"); }
    void undo() override {}
    void redo() override {}
};

// Builds a sketch with a single, non-fixed point at the origin.
struct SketchFixture
{
    cadalytic::Document document{"Command test document"};
    cadalytic::Part& part;
    cadalytic::Sketch& sketch;
    cadalytic::SketchPoint& point;

    SketchFixture()
        : part(document.addPart("Part")),
          sketch(part.addSketch("Sketch")),
          point(sketch.addPoint(0.0, 0.0, false))
    {
    }
};

void testExecutePushsUndoStack()
{
    SketchFixture fixture;
    cadalytic::CommandManager manager;

    require(manager.execute(std::make_unique<cadalytic::SetSketchPointPositionCommand>(
                fixture.sketch, fixture.point.id(), 5.0, 7.0)),
            "execute should succeed");
    require(manager.undoDepth() == 1, "successful execute should push one undo step");
    require(manager.redoDepth() == 0, "redo stack should be empty after execute");
    require(manager.canUndo(), "canUndo should be true after execute");
    require(!manager.canRedo(), "canRedo should be false after execute");
    require(std::abs(fixture.point.x() - 5.0) < 1e-9
              && std::abs(fixture.point.y() - 7.0) < 1e-9,
            "execute should commit the new position");
}

void testUndoRevertsAndRedoReapplies()
{
    SketchFixture fixture;
    cadalytic::CommandManager manager;
    manager.execute(std::make_unique<cadalytic::SetSketchPointPositionCommand>(
        fixture.sketch, fixture.point.id(), 3.0, 4.0));

    require(manager.undo(), "undo should succeed");
    require(std::abs(fixture.point.x()) < 1e-9 && std::abs(fixture.point.y()) < 1e-9,
            "undo should revert the position to its captured old value");
    require(manager.redoDepth() == 1 && manager.undoDepth() == 0,
            "undo should move the command to the redo stack");

    require(manager.redo(), "redo should succeed");
    require(std::abs(fixture.point.x() - 3.0) < 1e-9
              && std::abs(fixture.point.y() - 4.0) < 1e-9,
            "redo should re-apply the new position");
    require(manager.undoDepth() == 1 && manager.redoDepth() == 0,
            "redo should move the command back to the undo stack");
}

void testNewCommandClearsRedoStack()
{
    SketchFixture fixture;
    cadalytic::CommandManager manager;

    // AddSketchPointCommand is not mergeable, so each execute is its own undo
    // step regardless of what came before (unlike point-move commands, which
    // share a merge key and coalesce).
    manager.execute(std::make_unique<cadalytic::AddSketchPointCommand>(
        fixture.sketch, 1.0, 1.0));
    manager.execute(std::make_unique<cadalytic::AddSketchPointCommand>(
        fixture.sketch, 2.0, 2.0));

    require(manager.undoDepth() == 2, "two executes should yield two undo steps");
    require(manager.undo(), "undo should succeed");
    require(manager.redoDepth() == 1, "undo should populate the redo stack");

    // A fresh command after an undo must abandon the redo branch (timeline
    // branched), exactly like a text editor.
    manager.execute(std::make_unique<cadalytic::AddSketchPointCommand>(
        fixture.sketch, 9.0, 9.0));
    require(manager.redoDepth() == 0, "new command after undo should clear the redo stack");
    require(manager.undoDepth() == 2, "new command should add an undo step");
    require(manager.canUndo(), "canUndo should hold after a branched execute");
    require(!manager.canRedo(), "canRedo should be false after a branched execute");
}

void testNullCommandFails()
{
    SketchFixture fixture;
    cadalytic::CommandManager manager;
    require(!manager.execute(nullptr), "null command should fail");
    require(!manager.lastError().empty(), "failure should record a lastError");
    require(manager.undoDepth() == 0, "failed command should not reach the undo stack");
    require(!manager.canUndo(), "nothing should be undoable");
}

void testFailingCommandStaysOffUndoStack()
{
    SketchFixture fixture;
    cadalytic::CommandManager manager;
    require(!manager.execute(std::make_unique<FailCommand>()),
            "a command that sets an error should fail");
    require(manager.lastError() == "intentional failure",
            "lastError should mirror the command's error");
    require(manager.undoDepth() == 0, "failing command should not be pushed onto the undo stack");
}

void testMergeableCommandsCoalesce()
{
    // Dragging a point issues many move commands; consecutive moves on the
    // same point must collapse into a single undo step.
    SketchFixture fixture;
    cadalytic::CommandManager manager;

    manager.execute(std::make_unique<cadalytic::SetSketchPointPositionCommand>(
        fixture.sketch, fixture.point.id(), 3.0, 0.0));
    require(manager.undoDepth() == 1, "first move should push one undo step");

    manager.execute(std::make_unique<cadalytic::SetSketchPointPositionCommand>(
        fixture.sketch, fixture.point.id(), 6.0, 0.0));
    require(manager.undoDepth() == 1, "mergeable move should fold into the existing step");
    require(std::abs(fixture.point.x() - 6.0) < 1e-9, "merged execute should apply to the new value");

    // Undoing the merged step reverts to the position captured by the FIRST
    // command (the one that introduced the merge key).
    require(manager.undo(), "undo of merged step should succeed");
    require(std::abs(fixture.point.x()) < 1e-9, "undo should revert to the original captured position");
}

void testClearEmptiesStacks()
{
    SketchFixture fixture;
    cadalytic::CommandManager manager;
    manager.execute(std::make_unique<cadalytic::SetSketchPointPositionCommand>(
        fixture.sketch, fixture.point.id(), 4.0, 4.0));
    require(manager.undoDepth() == 1, "stack should hold one step before clear");
    manager.undo();
    require(manager.redoDepth() == 1, "undo should populate redo before clear");
    manager.clear();
    require(manager.undoDepth() == 0, "clear should empty the undo stack");
    require(manager.redoDepth() == 0, "clear should empty the redo stack");
}

void testEventBusNotification()
{
    SketchFixture fixture;
    cadalytic::CommandManager manager;
    cadalytic::EventBus bus;
    manager.setEventBus(bus);

    int executed = 0;
    int undid = 0;
    int redid = 0;
    bus.subscribe([&](const cadalytic::Event& e) {
        if (e.type == cadalytic::EventType::CommandExecuted) {
            ++executed;
        }
    });
    bus.subscribe([&](const cadalytic::Event& e) {
        if (e.type == cadalytic::EventType::UndoPerformed) {
            ++undid;
        }
    });
    bus.subscribe([&](const cadalytic::Event& e) {
        if (e.type == cadalytic::EventType::RedoPerformed) {
            ++redid;
        }
    });

    manager.execute(std::make_unique<cadalytic::SetSketchPointPositionCommand>(
        fixture.sketch, fixture.point.id(), 2.0, 2.0));
    require(executed == 1, "execute should publish a CommandExecuted event");

    manager.undo();
    require(undid == 1, "undo should publish an UndoPerformed event");

    manager.redo();
    require(redid == 1, "redo should publish a RedoPerformed event");
}

} // namespace

int main()
{
    testExecutePushsUndoStack();
    testUndoRevertsAndRedoReapplies();
    testNewCommandClearsRedoStack();
    testNullCommandFails();
    testFailingCommandStaysOffUndoStack();
    testMergeableCommandsCoalesce();
    testClearEmptiesStacks();
    testEventBusNotification();
    std::cout << "All command tests passed.\n";
    return EXIT_SUCCESS;
}
