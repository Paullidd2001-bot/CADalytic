#include "Part.h"

#include <algorithm>
#include <utility>

namespace cadalytic {

Part::Part(std::uint64_t id, std::string name)
    : m_id(id),
      m_name(std::move(name))
{
}

Feature& Part::addFeature(std::string name, FeatureType type)
{
    auto feature = std::make_unique<Feature>(m_nextFeatureId++, std::move(name), type);
    m_features.push_back(std::move(feature));
    return *m_features.back();
}

Sketch& Part::addSketch(std::string name)
{
    auto sketch = std::make_unique<Sketch>(m_nextFeatureId++, std::move(name));
    auto* sketchPtr = sketch.get();
    m_features.push_back(std::move(sketch));
    return *sketchPtr;
}

bool Part::removeFeature(std::uint64_t featureId)
{
    const auto it = std::find_if(
        m_features.begin(), m_features.end(),
        [featureId](const std::unique_ptr<Feature>& feature) {
            return feature->id() == featureId;
        });
    if (it == m_features.end()) {
        return false;
    }
    m_features.erase(it);
    return true;
}

} // namespace cadalytic
