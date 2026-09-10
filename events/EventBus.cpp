#include "EventBus.h"

#include <algorithm>
#include <utility>

namespace cadalytic {

std::uint64_t EventBus::subscribe(std::function<void(const Event&)> handler)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    const std::uint64_t id = m_nextId++;
    m_entries.push_back(Entry{id, std::move(handler)});
    return id;
}

bool EventBus::unsubscribe(std::uint64_t subscriptionId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    const auto it = std::find_if(
        m_entries.begin(), m_entries.end(),
        [subscriptionId](const Entry& entry) { return entry.id == subscriptionId; });
    if (it == m_entries.end()) {
        return false;
    }
    m_entries.erase(it);
    return true;
}

void EventBus::publish(const Event& event)
{
    std::vector<Entry> snapshot;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        snapshot = m_entries;
    }
    for (const auto& entry : snapshot) {
        entry.handler(event);
    }
}

void EventBus::clear()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_entries.clear();
}

std::size_t EventBus::subscriberCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_entries.size();
}

} // namespace cadalytic