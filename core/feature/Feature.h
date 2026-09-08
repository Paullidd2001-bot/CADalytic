#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace cadalytic {

class GeomShape;
using GeomShapePtr = std::shared_ptr<GeomShape>;

enum class FeatureType
{
    Generic,
    Sketch,
    Solid
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
    std::vector<std::uint64_t> m_dependencies;
    bool m_dirty = true;
};

} // namespace cadalytic
