#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>

#include "EventTypes.h"

namespace cadalytic {

// Component J: a thread-safe signal/slot bus.
//
// Subscribers register a handler and receive every published event. Handlers
// run synchronously on the publishing thread. The bus snapshots its handler
// list before dispatching, so subscribing or unsubscribing from inside a
// handler is safe and never affects the delivery of the current event.
class EventBus
{
public:
    std::uint64_t subscribe(std::function<void(const Event&)> handler);
    bool unsubscribe(std::uint64_t subscriptionId);
    void publish(const Event& event);
    void clear();
    std::size_t subscriberCount() const;

private:
    struct Entry
    {
        std::uint64_t id;
        std::function<void(const Event&)> handler;
    };

    std::uint64_t m_nextId = 1;
    std::vector<Entry> m_entries;
    mutable std::mutex m_mutex;
};

} // namespace cadalytic