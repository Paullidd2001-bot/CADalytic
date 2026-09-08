#include "DependencyGraph.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <unordered_set>

namespace cadalytic {

void DependencyGraph::addNode(std::uint64_t id)
{
    m_dependencies.try_emplace(id);
    m_dependents.try_emplace(id);
}

void DependencyGraph::addDependency(std::uint64_t featureId, std::uint64_t dependencyId)
{
    addNode(featureId);
    addNode(dependencyId);
    auto& dependencies = m_dependencies[featureId];
    if (std::find(dependencies.begin(), dependencies.end(), dependencyId) == dependencies.end()) {
        dependencies.push_back(dependencyId);
        m_dependents[dependencyId].push_back(featureId);
    }
}

void DependencyGraph::markDirty(std::uint64_t id)
{
    std::unordered_set<std::uint64_t> visited;
    std::vector<std::uint64_t> pending{id};
    while (!pending.empty()) {
        const auto current = pending.back();
        pending.pop_back();
        if (!visited.insert(current).second) {
            continue;
        }
        m_dirtyNodes.push_back(current);
        const auto dependentsIt = m_dependents.find(current);
        if (dependentsIt != m_dependents.end()) {
            pending.insert(pending.end(), dependentsIt->second.begin(), dependentsIt->second.end());
        }
    }
}

const std::vector<std::uint64_t>& DependencyGraph::dependents(std::uint64_t id) const
{
    static const std::vector<std::uint64_t> empty;
    const auto it = m_dependents.find(id);
    return it == m_dependents.end() ? empty : it->second;
}

std::vector<std::uint64_t> DependencyGraph::rebuildOrder() const
{
    m_error.clear();
    std::vector<std::uint64_t> order;
    std::unordered_map<std::uint64_t, int> state;

    std::function<bool(std::uint64_t)> visit = [&](std::uint64_t id) {
        const auto currentState = state[id];
        if (currentState == 1) {
            m_error = "Dependency cycle detected";
            return false;
        }
        if (currentState == 2) {
            return true;
        }
        state[id] = 1;
        const auto dependenciesIt = m_dependencies.find(id);
        if (dependenciesIt != m_dependencies.end()) {
            for (const auto dependencyId : dependenciesIt->second) {
                if (!visit(dependencyId)) {
                    return false;
                }
            }
        }
        state[id] = 2;
        order.push_back(id);
        return true;
    };

    for (const auto& [id, dependencies] : m_dependencies) {
        static_cast<void>(dependencies);
        if (!visit(id)) {
            return {};
        }
    }
    return order;
}

} // namespace cadalytic
