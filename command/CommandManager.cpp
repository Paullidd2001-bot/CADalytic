#include "CommandManager.h"

#include "../events/EventTypes.h"

namespace cadalytic {

namespace {
void notify(EventBus* bus, EventType type, const Command& command)
{
    if (bus != nullptr) {
        bus->publish(Event{type, command.id(), 0, command.name()});
    }
}
} // namespace

bool CommandManager::execute(std::unique_ptr<Command> command)
{
    m_lastError.clear();
    if (!command) {
        m_lastError = "Cannot execute a null command";
        return false;
    }

    command->execute();
    if (!command->error().empty()) {
        m_lastError = command->error();
        return false;
    }

    // Mergeable commands fold into the top of the undo stack.
    if (!m_undoStack.empty() && command->mergeable()
        && m_undoStack.back()->mergeable()
        && m_undoStack.back()->mergeKey() == command->mergeKey()
        && m_undoStack.back()->mergeKey() != 0) {
        m_undoStack.back()->merge(*command);
        notify(m_eventBus, EventType::CommandExecuted, *m_undoStack.back());
        return true;
    }

    m_undoStack.push_back(std::move(command));
    m_redoStack.clear();
    notify(m_eventBus, EventType::CommandExecuted, *m_undoStack.back());
    return true;
}

bool CommandManager::undo()
{
    m_lastError.clear();
    if (m_undoStack.empty()) {
        m_lastError = "Nothing to undo";
        return false;
    }

    auto command = std::move(m_undoStack.back());
    m_undoStack.pop_back();
    command->undo();
    if (!command->error().empty()) {
        m_lastError = command->error();
        m_undoStack.push_back(std::move(command));
        return false;
    }
    m_redoStack.push_back(std::move(command));
    notify(m_eventBus, EventType::UndoPerformed, *m_redoStack.back());
    return true;
}

bool CommandManager::redo()
{
    m_lastError.clear();
    if (m_redoStack.empty()) {
        m_lastError = "Nothing to redo";
        return false;
    }

    auto command = std::move(m_redoStack.back());
    m_redoStack.pop_back();
    command->redo();
    if (!command->error().empty()) {
        m_lastError = command->error();
        m_redoStack.push_back(std::move(command));
        return false;
    }
    m_undoStack.push_back(std::move(command));
    notify(m_eventBus, EventType::RedoPerformed, *m_undoStack.back());
    return true;
}

void CommandManager::clear()
{
    m_undoStack.clear();
    m_redoStack.clear();
    m_lastError.clear();
}

} // namespace cadalytic