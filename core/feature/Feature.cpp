#include "Feature.h"

#include <algorithm>
#include <utility>

namespace cadalytic {

void Feature::addDependency(std::uint64_t featureId)
{
    if (std::find(m_dependencies.begin(), m_dependencies.end(), featureId) == m_dependencies.end()) {
        m_dependencies.push_back(featureId);
        markDirty();
    }
}

Feature::Feature(std::uint64_t id, std::string name, FeatureType type)
    : m_id(id),
      m_name(std::move(name)),
      m_type(type)
{
}

} // namespace cadalytic
