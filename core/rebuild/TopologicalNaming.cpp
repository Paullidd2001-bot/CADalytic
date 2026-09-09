#include "TopologicalNaming.h"

#include <algorithm>
#include <limits>
#include <utility>

namespace cadalytic {

namespace {

std::string typeBase(TopoElementType type)
{
    switch (type) {
    case TopoElementType::Solid: return "Solid";
    case TopoElementType::Face: return "Face";
    case TopoElementType::Edge: return "Edge";
    case TopoElementType::Vertex: return "Vertex";
    }
    return "Element";
}

SemanticTag tagFromString(const std::string& semantic)
{
    if (semantic == "Top") return SemanticTag::Top;
    if (semantic == "Bottom") return SemanticTag::Bottom;
    if (semantic == "Front") return SemanticTag::Front;
    if (semantic == "Back") return SemanticTag::Back;
    if (semantic == "Left") return SemanticTag::Left;
    if (semantic == "Right") return SemanticTag::Right;
    if (semantic == "Origin") return SemanticTag::Origin;
    if (semantic == "Axis") return SemanticTag::Axis;
    return SemanticTag::None;
}

// Multiset overlap between two adjacency lists: count of common entries
// (a value present N times in A and M times in B contributes min(N, M)).
std::pair<std::size_t, std::size_t> adjacencyOverlap(
    const std::vector<std::string>& a,
    const std::vector<std::string>& b)
{
    std::unordered_map<std::string, std::size_t> countsA;
    for (const auto& entry : a) {
        ++countsA[entry];
    }
    std::size_t shared = 0;
    std::unordered_map<std::string, std::size_t> seen;
    for (const auto& entry : b) {
        if (countsA[entry] > seen[entry]) {
            ++shared;
            ++seen[entry];
        }
    }
    return {shared, a.size() + b.size() - shared};
}

} // namespace

TopoElementName::TopoElementName(std::uint64_t id,
                                 std::string name,
                                 TopoElementType type,
                                 std::string signature,
                                 std::string shapeType,
                                 std::vector<std::string> adjacency,
                                 SemanticTag tag)
    : m_id(id),
      m_name(std::move(name)),
      m_type(type),
      m_signature(std::move(signature)),
      m_shapeType(std::move(shapeType)),
      m_adjacency(std::move(adjacency)),
      m_tag(tag)
{
}
std::vector<TopoElementName> TopologicalNaming::nameElements(
    std::uint64_t featureId,
    const std::vector<TopoElementFact>& facts)
{
    std::vector<TopoElementName>& current = m_names[featureId];
    std::unordered_map<std::string, std::uint64_t>& counter = m_nextIndex[featureId];

    for (auto& element : current) {
        element.setActive(false);
    }

    const std::size_t factCount = facts.size();
    std::vector<std::size_t> matchIndex(factCount, std::numeric_limits<std::size_t>::max());
    std::vector<bool> currentMatched(current.size(), false);

    // Pass 1: exact signature + type match. When signatures are duplicated,
    // adjacency selects the existing element with the closest neighborhood.
    for (std::size_t j = 0; j < factCount; ++j) {
        std::size_t bestIndex = std::numeric_limits<std::size_t>::max();
        std::size_t bestShared = 0;
        for (std::size_t i = 0; i < current.size(); ++i) {
            if (currentMatched[i] || current[i].type() != facts[j].type
                || current[i].signature() != facts[j].signature) {
                continue;
            }
            const std::size_t shared = adjacencyOverlap(
                current[i].adjacency(), facts[j].adjacency).first;
            if (bestIndex == std::numeric_limits<std::size_t>::max()
                || shared > bestShared) {
                bestIndex = i;
                bestShared = shared;
            }
        }
        if (bestIndex != std::numeric_limits<std::size_t>::max()) {
            matchIndex[j] = bestIndex;
            currentMatched[bestIndex] = true;
        }
    }

    // Pass 2: adjacency-based recovery for elements that moved. Jaccard-style
    // overlap on adjacency signatures; a candidate only wins when it shares at
    // least one neighbour and covers at least half of the union.
    for (std::size_t j = 0; j < factCount; ++j) {
        if (matchIndex[j] != std::numeric_limits<std::size_t>::max()) {
            continue;
        }
        std::size_t bestIndex = std::numeric_limits<std::size_t>::max();
        double bestScore = 0.0;
        for (std::size_t i = 0; i < current.size(); ++i) {
            const bool alreadyMatched = std::any_of(
                matchIndex.begin(), matchIndex.end(),
                [i](std::size_t used) { return used == i; });
            if (alreadyMatched || currentMatched[i] || current[i].type() != facts[j].type) {
                continue;
            }
            const auto [shared, unionSize] = adjacencyOverlap(
                current[i].adjacency(), facts[j].adjacency);
            if (shared == 0) {
                continue;
            }
            const double score = static_cast<double>(shared) / static_cast<double>(unionSize);
            if (score > bestScore) {
                bestScore = score;
                bestIndex = i;
            }
        }
        if (bestIndex != std::numeric_limits<std::size_t>::max() && bestScore >= 0.5) {
            matchIndex[j] = bestIndex;
            currentMatched[bestIndex] = true;
        }
    }

    // Create new names for facts that matched nothing. Counter only advances,
    // so names are never reused after retirement.
    std::vector<std::size_t> createdIndex(factCount, std::numeric_limits<std::size_t>::max());
    std::uint64_t nextId = 0;
    for (const auto& element : current) {
        nextId = std::max(nextId, element.id());
    }
    for (std::size_t j = 0; j < factCount; ++j) {
        if (matchIndex[j] != std::numeric_limits<std::size_t>::max()) {
            continue;
        }
        const std::uint64_t index = ++counter[typeBase(facts[j].type)];
        TopoElementName element;
        element.setId(++nextId);
        element.setName(typeBase(facts[j].type) + std::to_string(index));
        element.setType(facts[j].type);
        element.setSignature(facts[j].signature);
        element.setShapeType(facts[j].shapeType);
        element.setAdjacency(facts[j].adjacency);
        element.setTag(tagFromString(facts[j].semantic));
        element.setActive(true);
        createdIndex[j] = current.size();
        current.push_back(std::move(element));
    }

    std::vector<TopoElementName> result;
    result.reserve(factCount);
    for (std::size_t j = 0; j < factCount; ++j) {
        if (matchIndex[j] != std::numeric_limits<std::size_t>::max()) {
            TopoElementName& element = current[matchIndex[j]];
            element.setActive(true);
            element.setSignature(facts[j].signature);
            element.setShapeType(facts[j].shapeType);
            element.setAdjacency(facts[j].adjacency);
            if (!facts[j].semantic.empty()) {
                element.setTag(tagFromString(facts[j].semantic));
            }
            result.push_back(element);
        } else {
            result.push_back(current[createdIndex[j]]);
        }
    }
    return result;
}

bool TopologicalNaming::hasFeature(std::uint64_t featureId) const
{
    return m_names.find(featureId) != m_names.end();
}

const std::vector<TopoElementName>& TopologicalNaming::namesFor(std::uint64_t featureId) const
{
    static const std::vector<TopoElementName> empty;
    const auto it = m_names.find(featureId);
    return it == m_names.end() ? empty : it->second;
}

const TopoElementName* TopologicalNaming::find(std::uint64_t featureId,
                                               TopoElementType type,
                                               const std::string& name) const
{
    const auto it = m_names.find(featureId);
    if (it == m_names.end()) {
        return nullptr;
    }
    for (const auto& element : it->second) {
        if (element.active() && element.type() == type && element.name() == name) {
            return &element;
        }
    }
    return nullptr;
}

const TopoElementName* TopologicalNaming::findByTag(std::uint64_t featureId,
                                                    TopoElementType type,
                                                    SemanticTag tag) const
{
    if (tag == SemanticTag::None) {
        return nullptr;
    }
    const auto it = m_names.find(featureId);
    if (it == m_names.end()) {
        return nullptr;
    }
    for (const auto& element : it->second) {
        if (element.active() && element.type() == type && element.tag() == tag) {
            return &element;
        }
    }
    return nullptr;
}

const TopoElementName* TopologicalNaming::findBySignature(std::uint64_t featureId,
                                                          TopoElementType type,
                                                          const std::string& signature) const
{
    const auto it = m_names.find(featureId);
    if (it == m_names.end()) {
        return nullptr;
    }
    for (const auto& element : it->second) {
        if (element.active() && element.type() == type && element.signature() == signature) {
            return &element;
        }
    }
    return nullptr;
}

void TopologicalNaming::clearFeature(std::uint64_t featureId)
{
    m_names.erase(featureId);
    m_nextIndex.erase(featureId);
}

void TopologicalNaming::clear()
{
    m_names.clear();
    m_nextIndex.clear();
}

} // namespace cadalytic