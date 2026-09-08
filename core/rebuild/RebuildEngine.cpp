#include "RebuildEngine.h"

#include <algorithm>

#include "../dependency/DependencyGraph.h"
#include "../document/Part.h"

namespace cadalytic {

RebuildResult RebuildEngine::rebuild(Part& part) const
{
    DependencyGraph graph;
    for (const auto& feature : part.features()) {
        graph.addNode(feature->id());
        for (const auto dependencyId : feature->dependencies()) {
            graph.addDependency(feature->id(), dependencyId);
        }
    }
    for (const auto& feature : part.features()) {
        if (feature->isDirty()) {
            graph.markDirty(feature->id());
        }
    }
    for (const auto dirtyId : graph.dirtyNodes()) {
        const auto featureIt = std::find_if(
            part.features().begin(),
            part.features().end(),
            [dirtyId](const auto& feature) { return feature->id() == dirtyId; });
        if (featureIt != part.features().end()) {
            (*featureIt)->markDirty();
        }
    }

    const auto order = graph.rebuildOrder();
    if (order.empty() && !part.features().empty()) {
        return {false, {}, graph.error()};
    }

    RebuildResult result{true, {}, {}};
    for (const auto featureId : order) {
        auto featureIt = std::find_if(
            part.features().begin(),
            part.features().end(),
            [featureId](const auto& feature) { return feature->id() == featureId; });
        if (featureIt == part.features().end() || !(*featureIt)->isDirty()) {
            continue;
        }
        if (!(*featureIt)->rebuild()) {
            result.success = false;
            result.error = "Feature rebuild failed: " + (*featureIt)->name();
            return result;
        }
        (*featureIt)->clearDirty();
        result.rebuiltFeatureIds.push_back(featureId);
    }
    return result;
}

} // namespace cadalytic
