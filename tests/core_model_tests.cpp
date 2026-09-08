#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>

#include "dependency/DependencyGraph.h"
#include "document/Document.h"
#include "rebuild/RebuildEngine.h"

namespace {

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void testDocumentHierarchy()
{
    cadalytic::Document document("Test document");
    auto& part = document.addPart("Part");
    auto& sketch = part.addSketch("Sketch");
    auto& constraint = sketch.addConstraint(cadalytic::ConstraintType::Distance, 25.0);

    require(document.parts().size() == 1, "document should contain one part");
    require(part.features().size() == 1, "part should contain one sketch");
    require(sketch.type() == cadalytic::FeatureType::Sketch, "feature should be a sketch");
    require(sketch.constraints().size() == 1, "sketch should contain one constraint");
    require(constraint.value() == 25.0, "constraint value should be retained");
}

void testDependencyOrderingAndDirtyPropagation()
{
    cadalytic::DependencyGraph graph;
    graph.addDependency(3, 2);
    graph.addDependency(2, 1);

    const auto order = graph.rebuildOrder();
    require(std::find(order.begin(), order.end(), 1) < std::find(order.begin(), order.end(), 2),
            "dependencies should precede dependents");
    require(std::find(order.begin(), order.end(), 2) < std::find(order.begin(), order.end(), 3),
            "transitive dependencies should be ordered");

    graph.markDirty(1);
    const auto& dirty = graph.dirtyNodes();
    require(std::find(dirty.begin(), dirty.end(), 1) != dirty.end(), "source should be dirty");
    require(std::find(dirty.begin(), dirty.end(), 2) != dirty.end(), "dependent should be dirty");
    require(std::find(dirty.begin(), dirty.end(), 3) != dirty.end(), "transitive dependent should be dirty");
}

void testCycleDetection()
{
    cadalytic::DependencyGraph graph;
    graph.addDependency(1, 2);
    graph.addDependency(2, 1);

    require(graph.rebuildOrder().empty(), "cyclic graph should have no rebuild order");
    require(graph.error() == "Dependency cycle detected", "cycle error should be reported");
}

void testRebuildAndCleanState()
{
    cadalytic::Document document("Rebuild document");
    auto& part = document.addPart("Part");
    auto& first = part.addFeature("First");
    auto& second = part.addFeature("Second");
    second.addDependency(first.id());

    cadalytic::RebuildEngine engine;
    const auto firstResult = engine.rebuild(part);
    require(firstResult.success, "initial rebuild should succeed");
    require(firstResult.rebuiltFeatureIds.size() == 2, "initial rebuild should rebuild both features");

    const auto cleanResult = engine.rebuild(part);
    require(cleanResult.success, "clean rebuild should succeed");
    require(cleanResult.rebuiltFeatureIds.empty(), "clean rebuild should do no work");

    first.markDirty();
    const auto dirtyResult = engine.rebuild(part);
    require(dirtyResult.rebuiltFeatureIds.size() == 2,
            "dirty source should rebuild its dependent feature");
}

} // namespace

int main()
{
    testDocumentHierarchy();
    testDependencyOrderingAndDirtyPropagation();
    testCycleDetection();
    testRebuildAndCleanState();
    std::cout << "All core model tests passed.\n";
    return EXIT_SUCCESS;
}
