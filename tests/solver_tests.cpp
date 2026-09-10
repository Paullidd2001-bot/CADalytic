#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "core/document/Document.h"
#include "solver/sketch/DOFAnalyzer.h"
#include "solver/sketch/SketchSolver.h"

namespace {

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

struct SketchFixture
{
    cadalytic::Document document;
    cadalytic::Sketch& sketch;

    SketchFixture(const std::string& name)
        : document("Solver test document"),
          sketch(document.addPart("Part").addSketch(name))
    {
    }
};

void testDistanceConstraintMovesPoint()
{
    SketchFixture fixture("Distance");
    const auto& anchor = fixture.sketch.addPoint(0.0, 0.0, true);
    const auto& free = fixture.sketch.addPoint(3.0, 0.0, false);
    fixture.sketch.addConstraint(cadalytic::ConstraintType::Distance, 5.0,
                                 {anchor.id(), free.id()});

    cadalytic::SketchSolver solver;
    const auto result = solver.solve(fixture.sketch);
    require(result.success, "distance solve should succeed: " + result.error);

    const double dx = free.x() - anchor.x();
    const double dy = free.y() - anchor.y();
    require(std::abs(std::hypot(dx, dy) - 5.0) < 1e-6,
            "free point should end up 5 units from the anchor");
}

void testHorizontalConstraintAlignsY()
{
    SketchFixture fixture("Horizontal");
    const auto& p1 = fixture.sketch.addPoint(0.0, 0.0, false);
    const auto& p2 = fixture.sketch.addPoint(3.0, 7.0, false);
    fixture.sketch.addConstraint(cadalytic::ConstraintType::Horizontal,
                                 0.0, {p1.id(), p2.id()});

    cadalytic::SketchSolver solver;
    const auto result = solver.solve(fixture.sketch);
    require(result.success, "horizontal solve should succeed: " + result.error);
    require(std::abs(p1.y() - p2.y()) < 1e-6,
            "points should share the same y after a horizontal constraint");
}

void testVerticalConstraintAlignsX()
{
    SketchFixture fixture("Vertical");
    const auto& p1 = fixture.sketch.addPoint(5.0, 0.0, false);
    const auto& p2 = fixture.sketch.addPoint(1.0, 2.0, false);
    fixture.sketch.addConstraint(cadalytic::ConstraintType::Vertical,
                                 0.0, {p1.id(), p2.id()});

    cadalytic::SketchSolver solver;
    const auto result = solver.solve(fixture.sketch);
    require(result.success, "vertical solve should succeed: " + result.error);
    require(std::abs(p1.x() - p2.x()) < 1e-6,
            "points should share the same x after a vertical constraint");
}

void testCoincidentConstraintJoinsPoints()
{
    SketchFixture fixture("Coincident");
    const auto& p1 = fixture.sketch.addPoint(1.0, 1.0, false);
    const auto& p2 = fixture.sketch.addPoint(6.0, -2.0, false);
    fixture.sketch.addConstraint(cadalytic::ConstraintType::Coincident,
                                 0.0, {p1.id(), p2.id()});

    cadalytic::SketchSolver solver;
    const auto result = solver.solve(fixture.sketch);
    require(result.success, "coincident solve should succeed: " + result.error);
    require(std::abs(p1.x() - p2.x()) < 1e-6 && std::abs(p1.y() - p2.y()) < 1e-6,
            "coincident points should converge to one position");
}

void testFullyConstrainedRectangle()
{
    SketchFixture fixture("Rectangle");
    const auto& p0 = fixture.sketch.addPoint(0.0, 0.0, true);
    const auto& p1 = fixture.sketch.addPoint(1.0, 0.5, false);
    const auto& p2 = fixture.sketch.addPoint(1.5, 1.5, false);
    const auto& p3 = fixture.sketch.addPoint(0.5, 1.0, false);

    fixture.sketch.addConstraint(cadalytic::ConstraintType::Horizontal, 0.0, {p0.id(), p1.id()});
    fixture.sketch.addConstraint(cadalytic::ConstraintType::Vertical, 0.0, {p1.id(), p2.id()});
    fixture.sketch.addConstraint(cadalytic::ConstraintType::Horizontal, 0.0, {p2.id(), p3.id()});
    fixture.sketch.addConstraint(cadalytic::ConstraintType::Vertical, 0.0, {p0.id(), p3.id()});
    fixture.sketch.addConstraint(cadalytic::ConstraintType::Distance, 4.0, {p0.id(), p1.id()});
    fixture.sketch.addConstraint(cadalytic::ConstraintType::Distance, 3.0, {p0.id(), p3.id()});
    fixture.sketch.addConstraint(cadalytic::ConstraintType::Distance, 3.0, {p1.id(), p2.id()});
    fixture.sketch.addConstraint(cadalytic::ConstraintType::Distance, 4.0, {p3.id(), p2.id()});

    cadalytic::SketchSolver solver;
    const auto result = solver.solve(fixture.sketch);
    require(result.success, "rectangle solve should succeed: " + result.error);

    const double tolerance = 1e-4;
    require(std::abs(p1.x() - 4.0) < tolerance && std::abs(p1.y()) < tolerance,
            "p1 should be (4, 0) in a 4x3 rectangle");
    require(std::abs(p2.x() - 4.0) < tolerance && std::abs(p2.y() - 3.0) < tolerance,
            "p2 should be (4, 3) in a 4x3 rectangle");
    require(std::abs(p3.x()) < tolerance && std::abs(p3.y() - 3.0) < tolerance,
            "p3 should be (0, 3) in a 4x3 rectangle");
    require(std::abs(p0.x()) < tolerance && std::abs(p0.y()) < tolerance,
            "fixed point should not move");
}