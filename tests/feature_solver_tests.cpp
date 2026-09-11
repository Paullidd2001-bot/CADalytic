#include <cstdlib>
#include <iostream>
#include <string>

#include "document/Document.h"
#include "sketch/Sketch.h"
#include "features/FeatureSolver.h"

namespace {

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

// A closed 10x10 rectangle in the XY plane.
void buildClosedSketch(cadalytic::Sketch& sketch)
{
    auto& p1 = sketch.addPoint(0.0, 0.0, true);
    auto& p2 = sketch.addPoint(10.0, 0.0);
    auto& p3 = sketch.addPoint(10.0, 10.0);
    auto& p4 = sketch.addPoint(0.0, 10.0);
    sketch.addLine(p1.id(), p2.id());
    sketch.addLine(p2.id(), p3.id());
    sketch.addLine(p3.id(), p4.id());
    sketch.addLine(p4.id(), p1.id());
}

void testBoxSolve()
{
    cadalytic::Document doc("box");
    auto& part = doc.addPart("Part");
    auto& box = part.addFeature("Base", cadalytic::FeatureType::Box);
    box.setParam("width", 10.0);
    box.setParam("depth", 20.0);
    box.setParam("height", 30.0);

    cadalytic::FeatureSolver solver;
    const auto result = solver.solve(part);
    require(result.success, "box solve should succeed: " + result.error);
    require(result.rebuiltFeatureIds.size() == 1, "one feature rebuilt");
    const auto it = solver.shapes().find(box.id());
    require(it != solver.shapes().end(), "box shape should be present");
    require(!it->second.IsNull(), "box shape should be valid");
}

void testCleanSolveDoesNothing()
{
    cadalytic::Document doc("clean");
    auto& part = doc.addPart("Part");
    auto& box = part.addFeature("Base", cadalytic::FeatureType::Box);
    box.setParam("width", 10.0);

    cadalytic::FeatureSolver solver;
    require(solver.solve(part).success, "initial solve should succeed");

    const auto second = solver.solve(part);
    require(second.success, "clean solve should succeed");
    require(second.rebuiltFeatureIds.empty(), "clean solve should rebuild nothing");
}

void testParamChangeRetriggersRebuild()
{
    cadalytic::Document doc("retrigger");
    auto& part = doc.addPart("Part");
    auto& box = part.addFeature("Base", cadalytic::FeatureType::Box);
    box.setParam("width", 10.0);

    cadalytic::FeatureSolver solver;
    solver.solve(part);

    box.setParam("height", 5.0); // marks the feature dirty
    const auto result = solver.solve(part);
    require(result.success, "re-solve should succeed");
    require(result.rebuiltFeatureIds.size() == 1,
            "parameter change should trigger a rebuild");
}
void testSketchExtrude()
{
    cadalytic::Document doc("extrude");
    auto& part = doc.addPart("Part");
    auto& sketch = part.addSketch("Profile");
    buildClosedSketch(sketch);

    auto& extrude = part.addFeature("Pad", cadalytic::FeatureType::Extrude);
    extrude.addDependency(sketch.id());
    extrude.setParam("depth", 10.0);

    cadalytic::FeatureSolver solver;
    const auto result = solver.solve(part);
    require(result.success, "extrude solve should succeed: " + result.error);
    const auto it = solver.shapes().find(extrude.id());
    require(it != solver.shapes().end(), "extruded shape should be present");
    require(!it->second.IsNull(), "extruded shape should be valid");
}

void testRevolve()
{
    cadalytic::Document doc("revolve");
    auto& part = doc.addPart("Part");
    auto& sketch = part.addSketch("Profile");
    buildClosedSketch(sketch);

    auto& revolve = part.addFeature("Lathe", cadalytic::FeatureType::Revolve);
    revolve.addDependency(sketch.id());
    revolve.setParam("angle", 360.0);

    cadalytic::FeatureSolver solver;
    const auto result = solver.solve(part);
    require(result.success, "revolve solve should succeed: " + result.error);
    const auto it = solver.shapes().find(revolve.id());
    require(it != solver.shapes().end(), "revolved shape should be present");
    require(!it->second.IsNull(), "revolved shape should be valid");
}
void testBooleanCut()
{
    cadalytic::Document doc("cut");
    auto& part = doc.addPart("Part");
    auto& box = part.addFeature("Base", cadalytic::FeatureType::Box);
    box.setParam("width", 20.0);
    box.setParam("depth", 20.0);
    box.setParam("height", 20.0);

    auto& cylinder = part.addFeature("Hole", cadalytic::FeatureType::Cylinder);
    cylinder.setParam("radius", 3.0);
    cylinder.setParam("height", 25.0);
    cylinder.setParam("tx", 10.0);
    cylinder.setParam("ty", 10.0);
    cylinder.setParam("tz", -2.0);

    auto& cut = part.addFeature("Cut", cadalytic::FeatureType::Cut);
    cut.addDependency(box.id());
    cut.addDependency(cylinder.id());

    cadalytic::FeatureSolver solver;
    const auto result = solver.solve(part);
    require(result.success, "cut solve should succeed: " + result.error);
    const auto it = solver.shapes().find(cut.id());
    require(it != solver.shapes().end(), "cut shape should be present");
    require(!it->second.IsNull(), "cut shape should be valid");
}

void testFillet()
{
    cadalytic::Document doc("fillet");
    auto& part = doc.addPart("Part");
    auto& box = part.addFeature("Base", cadalytic::FeatureType::Box);
    box.setParam("width", 10.0);
    box.setParam("depth", 10.0);
    box.setParam("height", 10.0);

    auto& fillet = part.addFeature("Round", cadalytic::FeatureType::Fillet);
    fillet.addDependency(box.id());
    fillet.setParam("radius", 1.0);

    cadalytic::FeatureSolver solver;
    const auto result = solver.solve(part);
    require(result.success, "fillet solve should succeed: " + result.error);
    const auto it = solver.shapes().find(fillet.id());
    require(it != solver.shapes().end(), "fillet shape should be present");
}

void testFailedFeatureReportsError()
{
    cadalytic::Document doc("failure");
    auto& part = doc.addPart("Part");
    // Extrude with no sketch dependency must fail cleanly.
    auto& extrude = part.addFeature("Orphan", cadalytic::FeatureType::Extrude);
    extrude.setParam("depth", 5.0);

    cadalytic::FeatureSolver solver;
    const auto result = solver.solve(part);
    require(!result.success, "orphan extrude should fail");
    require(!result.error.empty(), "an error message should be produced");
}

} // namespace

int main()
{
    testBoxSolve();
    testCleanSolveDoesNothing();
    testParamChangeRetriggersRebuild();
    testSketchExtrude();
    testRevolve();
    testBooleanCut();
    testFillet();
    testFailedFeatureReportsError();
    std::cout << "All feature solver tests passed.\n";
    return EXIT_SUCCESS;
}