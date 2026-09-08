#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "../feature/Feature.h"
#include "../sketch/Sketch.h"

namespace cadalytic {

class Part
{
public:
    Part(std::uint64_t id, std::string name);

    std::uint64_t id() const { return m_id; }
    const std::string& name() const { return m_name; }
    void setName(std::string name) { m_name = std::move(name); }

    Feature& addFeature(std::string name, FeatureType type = FeatureType::Generic);
    Sketch& addSketch(std::string name);
    const std::vector<std::unique_ptr<Feature>>& features() const { return m_features; }

private:
    std::uint64_t m_id;
    std::string m_name;
    std::uint64_t m_nextFeatureId = 1;
    std::vector<std::unique_ptr<Feature>> m_features;
};

} // namespace cadalytic
