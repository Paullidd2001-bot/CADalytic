#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../events/EventBus.h"
#include "Command.h"

namespace cadalytic {

// Component I: executes commands and maintains undo/redo stacks.
//
// execute() runs the command and pushes it onto the undo stack (clearing the
// redo stack, as the timeline has branched). Commands that merge with the top
// of the stack fold into it instead of creating a new entry. undo()/redo()
// replay commands in stack order.
//
// When an EventBus is attached, every successful operation publishes a
// CommandExecuted / UndoPerformed / RedoPerformed event so the UI can refresh.
class CommandManager
{
public:
    bool execute(std::unique_ptr<Command> command);
    bool undo();
    bool redo();
    void clear();

    bool canUndo() const { return !m_undoStack.empty(); }
    bool canRedo() const { return !m_redoStack.empty(); }
    std::size_t undoDepth() const { return m_undoStack.size(); }
    std::size_t redoDepth() const { return m_redoStack.size(); }

    const std::string& lastError() const { return m_lastError; }

    void setEventBus(EventBus& bus) { m_eventBus = &bus; }
    EventBus* eventBus() const { return m_eventBus; }

private:
    std::vector<std::unique_ptr<Command>> m_undoStack;
    std::vector<std::unique_ptr<Command>> m_redoStack;
    std::string m_lastError;
    EventBus* m_eventBus = nullptr;
};

} // namespace cadalytic