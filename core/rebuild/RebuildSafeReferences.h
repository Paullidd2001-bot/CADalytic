#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../document/Part.h"
#include "TopologicalNaming.h"

namespace cadalytic {

enum class ReferenceScope
{
    Feature,     // the whole feature
    TopoElement, // a topological element addressed by its stable name (Component F)
    Geometry     // an element addressed by its geometric signature
};

// A single rebuild-safe reference. References never store raw pointers or
// volatile model indices; they describe their target declaratively so that
// resolution can be re-evaluated against the latest model state.
class RebuildSafeReference
{
public:
    RebuildSafeReference(std::uint64_t id,
                         std::uint64_t featureId,
                         ReferenceScope scope,
                         TopoElementType elementType,
                         std::string target);

    std::uint64_t id() const { return m_id; }
    std::uint64_t featureId() const { return m_featureId; }
    ReferenceScope scope() const { return m_scope; }
    TopoElementType elementType() const { return m_elementType; }
    const std::string& target() const { return m_target; }

    bool alive() const { return m_alive; }
    void setAlive(bool alive) { m_alive = alive; }

private:
    std::uint64_t m_id;
    std::uint64_t m_featureId;
    ReferenceScope m_scope;
    TopoElementType m_elementType;
    std::string m_target;
    bool m_alive = false;
};

// Component L: stable references to features and geometry, integrated with
// the topological naming system. Clients add declarative references before a
// rebuild; afterwards they run validate() to learn which references survived.
class RebuildSafeReferences
{
public:
    std::uint64_t addFeatureReference(std::uint64_t featureId);
    std::uint64_t addElementReference(std::uint64_t featureId,
                                      TopoElementType type,
                                      const std::string& name);
    std::uint64_t addGeometryReference(std::uint64_t featureId,
                                       TopoElementType type,
                                       const std::string& signature);

    bool remove(std::uint64_t referenceId);
    bool contains(std::uint64_t referenceId) const;
    const std::vector<RebuildSafeReference>& all() const { return m_references; }
    std::size_t count() const { return m_references.size(); }
    void clear();

    // Re-evaluates every reference against the current model state. Returns
    // the ids of references that no longer resolve so callers can react.
    std::vector<std::uint64_t> validate(const Part& part, const TopologicalNaming& naming);

private:
    std::uint64_t m_nextId = 1;
    std::vector<RebuildSafeReference> m_references;
};

} // namespace cadalytic