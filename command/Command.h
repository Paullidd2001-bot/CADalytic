#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace cadalytic {

// Component I: base class for every user action.
//
// A command records everything needed to reverse itself later, so undo/redo
// never depends on the model keeping its own history. Commands are
// declarative: they describe changes to the document model, and the model
// remains the single source of truth.
class Command
{
public:
    Command(std::string name);

    std::uint64_t id() const { return m_id; }
    const std::string& name() const { return m_name; }

    // Commands set an error description when they cannot complete their work.
    // The CommandManager treats a non-empty error as a failed operation and
    // keeps the command out of the undo stack.
    const std::string& error() const { return m_error; }
    void setError(std::string error) { m_error = std::move(error); }

    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;

    // Commands sharing a nonzero merge key can coalesce: dragging a sketch
    // point across many commands becomes a single undo step. merge() folds a
    // newer command's effect into this one.
    virtual std::uint64_t mergeKey() const { return 0; }
    virtual bool mergeable() const { return false; }
    virtual void merge(const Command&) {}

private:
    std::uint64_t m_id;
    std::string m_name;
    std::string m_error;
};

} // namespace cadalytic