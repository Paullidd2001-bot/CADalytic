#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <TopoDS_Shape.hxx>

namespace cadalytic {

class Part;
class Sketch;
class Feature;

// Outcome of a solve pass over a part.
struct FeatureSolveResult
{
    bool success = false;
    std::string error;
    std::vector<std::uint64_t> rebuiltFeatureIds;
};

// Component R: constraint-driven 3D feature solver.
//
// Given a Part, the solver walks the feature history in dependency order and
// evaluates each feature into a TopoDS_Shape:
//
//   Sketch    -> sketch constraints are solved (component O), then the 2D
//                geometry is promoted to a planar face in the XY plane
//   Primitives-> Box / Cylinder / Sphere / Cone / Torus from parameters
//   Extrude   -> prism of a profile sketch's face along +Z ('depth')
//   Revolve   -> body of revolution of a profile sketch's face around Z
//                ('angle', degrees; full revolution when unset)
//   Loft      -> solid lofted through several profile sketches (dependencies)
//   Fillet    -> rounds every edge of the current solid ('radius')
//   Chamfer   -> bevels every edge of the current solid ('distance')
//   Shell     -> hollows the current solid ('thickness')
//   Fuse      -> union of two operand solids (dependencies)
//   Cut       -> subtraction of the second operand from the first
//   Common    -> intersection of two operand solids
//
// Evaluation is rebuild-aware: a feature is re-evaluated when it is dirty or
// any of its dependencies was dirty in the same pass. Clean sub-trees are
// untouched. The resulting shapes are exposed through shapes(); the shape for
// the last solid feature is the part body.
class FeatureSolver
{
public:
    FeatureSolveResult solve(Part& part);

    const std::map<std::uint64_t, TopoDS_Shape>& shapes() const { return m_shapes; }
    void clearShapes() { m_shapes.clear(); }

private:
    struct EvaluateContext;

    bool findFeature(const Part& part, std::uint64_t featureId,
                     Feature*& outFeature, std::string& error) const;
    bool isSolidType(FeatureType type) const;
    bool evaluate(Part& part, Feature& feature, std::string& error);
    bool evaluateSketch(const Sketch& sketch, TopoDS_Shape& outShape);
    bool currentSolid(TopoDS_Shape& outShape) const;
    bool dependencyShape(const Feature& feature, std::size_t index,
                         TopoDS_Shape& outShape, std::string& error) const;

    std::map<std::uint64_t, TopoDS_Shape> m_shapes;
    std::uint64_t m_lastSolidId = 0;
};

} // namespace cadalytic