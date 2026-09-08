#include "Part.h"

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

} // namespace cadalytic
