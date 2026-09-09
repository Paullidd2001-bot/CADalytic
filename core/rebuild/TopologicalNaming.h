#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cadalytic {

enum class TopoElementType
{
    Solid,
    Face,
    Edge,
    Vertex
};

// Semantic tags give stable, human-friendly meaning to topological elements
// (e.g. the face that faces +Z is "Top"). They survive signature changes as
// long as the element keeps the same geometric role.
enum class SemanticTag
{
    None,
    Top,
    Bottom,
    Front,
    Back,
    Left,
    Right,
    Origin,
    Axis
};

// Lightweight, geometry-derived facts describing one element of a rebuilt
// feature. The geometry layer (Component H) extracts these from the OCCT
// BRep; the core library never sees OCCT types. Two elements sharing a
// signature are geometrically interchangeable.
struct TopoElementFact
{
    TopoElementType type;
    std::string shapeType;               // e.g. "Plane", "Cylinder", "Line", "Point"
    std::string signature;               // canonical geometry string, e.g. "plane(0,0,1)|d=5"
    std::string semantic;                // optional hint, e.g. "Top"
    std::vector<std::string> adjacency;  // signatures of directly adjacent elements
};

// A stable, rebuild-safe name for one topological element (solid/face/edge/vertex).
class TopoElementName
{
public:
    TopoElementName() = default;

    TopoElementName(std::uint64_t id,
                    std::string name,
                    TopoElementType type,
                    std::string signature,
                    std::string shapeType,
                    std::vector<std::string> adjacency,
                    SemanticTag tag = SemanticTag::None);

    std::uint64_t id() const { return m_id; }
    const std::string& name() const { return m_name; }  // stable, e.g. "Face1"
    TopoElementType type() const { return m_type; }
    const std::string& signature() const { return m_signature; }
    const std::string& shapeType() const { return m_shapeType; }
    const std::vector<std::string>& adjacency() const { return m_adjacency; }
    SemanticTag tag() const { return m_tag; }
    bool active() const { return m_active; }

    void setId(std::uint64_t id) { m_id = id; }
    void setName(std::string name) { m_name = std::move(name); }
    void setType(TopoElementType type) { m_type = type; }
    void setSignature(std::string signature) { m_signature = std::move(signature); }
    void setShapeType(std::string shapeType) { m_shapeType = std::move(shapeType); }
    void setAdjacency(std::vector<std::string> adjacency) { m_adjacency = std::move(adjacency); }
    void setTag(SemanticTag tag) { m_tag = tag; }
    void setActive(bool active) { m_active = active; }

private:
    std::uint64_t m_id = 0;
    std::string m_name;
    TopoElementType m_type = TopoElementType::Face;
    std::string m_signature;
    std::string m_shapeType;
    std::vector<std::string> m_adjacency;
    SemanticTag m_tag = SemanticTag::None;
    bool m_active = false;
};

// Component F: stable face/edge/vertex identification across rebuilds.
//
// Each rebuild of a feature yields a fresh set of TopoElementFacts. When
// renamed, previously assigned names are reused wherever the geometric
// signature still matches; elements that moved (signature changed) are
// recovered through adjacency matching; genuinely new elements receive the
// next free index. Elements that disappear are retained but marked inactive
// so that references to them can distinguish "temporarily mid-rebuild" from
// "really deleted".
class TopologicalNaming
{
public:
    // Names (or renames) every element of a feature for the current rebuild.
    // The returned vector mirrors `facts` in order.
    std::vector<TopoElementName> nameElements(
        std::uint64_t featureId,
        const std::vector<TopoElementFact>& facts);

    bool hasFeature(std::uint64_t featureId) const;
    const std::vector<TopoElementName>& namesFor(std::uint64_t featureId) const;

    // Lookups resolve only active elements; returns nullptr when absent.
    const TopoElementName* find(std::uint64_t featureId, TopoElementType type,
                                const std::string& name) const;
    const TopoElementName* findByTag(std::uint64_t featureId, TopoElementType type,
                                     SemanticTag tag) const;
    const TopoElementName* findBySignature(std::uint64_t featureId, TopoElementType type,
                                           const std::string& signature) const;

    void clearFeature(std::uint64_t featureId);
    void clear();
    std::size_t featureCount() const { return m_names.size(); }

private:
    std::unordered_map<std::uint64_t, std::vector<TopoElementName>> m_names;
    std::unordered_map<std::uint64_t, std::unordered_map<std::string, std::uint64_t>> m_nextIndex;
};

} // namespace cadalytic