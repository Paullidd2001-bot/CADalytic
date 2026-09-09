#include "RebuildSafeReferences.h"

#include <algorithm>
#include <utility>

namespace cadalytic {

RebuildSafeReference::RebuildSafeReference(std::uint64_t id,
                                           std::uint64_t featureId,
                                           ReferenceScope scope,
                                           TopoElementType elementType,
                                           std::string target)
    : m_id(id),
      m_featureId(featureId),
      m_scope(scope),
      m_elementType(elementType),
      m_target(std::move(target))
{
}

std::uint64_t RebuildSafeReferences::addFeatureReference(std::uint64_t featureId)
{
    m_references.emplace_back(m_nextId++, featureId, ReferenceScope::Feature,
                              TopoElementType::Face, std::string());
    return m_references.back().id();
}

std::uint64_t RebuildSafeReferences::addElementReference(std::uint64_t featureId,
                                                         TopoElementType type,
                                                         const std::string& name)
{
    m_references.emplace_back(m_nextId++, featureId, ReferenceScope::TopoElement,
                              type, name);
    return m_references.back().id();
}

std::uint64_t RebuildSafeReferences::addGeometryReference(std::uint64_t featureId,
                                                          TopoElementType type,
                                                          const std::string& signature)
{
    m_references.emplace_back(m_nextId++, featureId, ReferenceScope::Geometry,
                              type, signature);
    return m_references.back().id();
}

bool RebuildSafeReferences::remove(std::uint64_t referenceId)
{
    const auto it = std::find_if(
        m_references.begin(), m_references.end(),
        [referenceId](const RebuildSafeReference& reference) {
            return reference.id() == referenceId;
        });
    if (it == m_references.end()) {
        return false;
    }
    m_references.erase(it);
    return true;
}

bool RebuildSafeReferences::contains(std::uint64_t referenceId) const
{
    return std::any_of(
        m_references.begin(), m_references.end(),
        [referenceId](const RebuildSafeReference& reference) {
            return reference.id() == referenceId;
        });
}

void RebuildSafeReferences::clear()
{
    m_references.clear();
}

std::vector<std::uint64_t> RebuildSafeReferences::validate(const Part& part,
                                                           const TopologicalNaming& naming)
{
    std::vector<std::uint64_t> dead;
    for (auto& reference : m_references) {
        bool alive = false;
        switch (reference.scope()) {
        case ReferenceScope::Feature:
            alive = std::any_of(
                part.features().begin(), part.features().end(),
                [reference](const std::unique_ptr<Feature>& feature) {
                    return feature->id() == reference.featureId();
                });
            break;
        case ReferenceScope::TopoElement:
            alive = naming.find(reference.featureId(), reference.elementType(),
                                reference.target()) != nullptr;
            break;
        case ReferenceScope::Geometry:
            alive = naming.findBySignature(reference.featureId(), reference.elementType(),
                                           reference.target()) != nullptr;
            break;
        }
        reference.setAlive(alive);
        if (!alive) {
            dead.push_back(reference.id());
        }
    }
    return dead;
}

} // namespace cadalytic