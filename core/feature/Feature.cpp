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

void Feature::setParam(const std::string& name, double value)
{
    m_params[name] = value;
    markDirty();
}

bool Feature::hasParam(const std::string& name) const
{
    return m_params.find(name) != m_params.end();
}

double Feature::param(const std::string& name, double fallback) const
{
    const auto it = m_params.find(name);
    return it == m_params.end() ? fallback : it->second;
}

Feature::Feature(std::uint64_t id, std::string name, FeatureType type)
    : m_id(id),
      m_name(std::move(name)),
      m_type(type)
{
}

} // namespace cadalytic
