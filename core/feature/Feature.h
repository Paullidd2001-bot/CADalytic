#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace cadalytic {

class GeomShape;
using GeomShapePtr = std::shared_ptr<GeomShape>;

// Feature taxonomy. Phase 1 used Generic/Sketch/Solid; Phase 3 (Component R)
// extends the taxonomy with the concrete 3D feature kinds the FeatureSolver
// evaluates:
//   - Geometry primitives (Box, Cylinder, Sphere, Cone, Torus)
//   - Profile features (Extrude, Revolve, Loft) driven by a sketch dependency
//   - Modifiers (Fillet, Chamfer, Shell) applied to the accumulated solid
//   - Boolean operators (Fuse, Cut, Common) combining two operand solids
enum class FeatureType
{
    Generic,
    Sketch,
    Solid,

    // Phase 3: primitives
    Box,
    Cylinder,
    Sphere,
    Cone,
    Torus,

    // Phase 3: profile-based features
    Extrude,
    Revolve,
    Loft,

    // Phase 3: modifiers
    Fillet,
    Chamfer,
    Shell,

    // Phase 3: boolean operations
    Fuse,
    Cut,
    Common
};

class Feature
{
public:
    Feature(std::uint64_t id, std::string name, FeatureType type = FeatureType::Generic);

    std::uint64_t id() const { return m_id; }
    const std::string& name() const { return m_name; }
    void setName(std::string name) { m_name = std::move(name); }

    FeatureType type() const { return m_type; }
    void setType(FeatureType type) { m_type = type; }

    const GeomShapePtr& shape() const { return m_shape; }
    void setShape(GeomShapePtr shape) { m_shape = std::move(shape); }

    // --- Parameters ---------------------------------------------------------
    // Phase 3 features are parametric: depth/radius/height/angle/thickness...
    // Names follow the per-type conventions documented on the FeatureSolver.
    void setParam(const std::string& name, double value);
    bool hasParam(const std::string& name) const;
    double param(const std::string& name, double fallback = 0.0) const;
    const std::map<std::string, double>& params() const { return m_params; }

    void addDependency(std::uint64_t featureId);
    const std::vector<std::uint64_t>& dependencies() const { return m_dependencies; }

    bool isDirty() const { return m_dirty; }
    void markDirty() { m_dirty = true; }
    void clearDirty() { m_dirty = false; }
    virtual bool rebuild() { return true; }

private:
    std::uint64_t m_id;
    std::string m_name;
    FeatureType m_type;
    GeomShapePtr m_shape;
    std::map<std::string, double> m_params;
    std::vector<std::uint64_t> m_dependencies;
    bool m_dirty = true;
};

} // namespace cadalytic
