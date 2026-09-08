#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace cadalytic {

class DependencyGraph
{
public:
    void addNode(std::uint64_t id);
    void addDependency(std::uint64_t featureId, std::uint64_t dependencyId);
    void markDirty(std::uint64_t id);

    const std::vector<std::uint64_t>& dependents(std::uint64_t id) const;
    std::vector<std::uint64_t> rebuildOrder() const;
    const std::vector<std::uint64_t>& dirtyNodes() const { return m_dirtyNodes; }
    const std::string& error() const { return m_error; }

private:
    std::unordered_map<std::uint64_t, std::vector<std::uint64_t>> m_dependencies;
    std::unordered_map<std::uint64_t, std::vector<std::uint64_t>> m_dependents;
    std::vector<std::uint64_t> m_dirtyNodes;
    mutable std::string m_error;
};

} // namespace cadalytic
