#include "FeatureSolver.h"

#include <set>
#include <sstream>
#include <utility>

#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Standard_Failure.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_Real.hxx>

#include "../../core/dependency/DependencyGraph.h"
#include "../../core/document/Part.h"
#include "../../core/sketch/Sketch.h"
#include "../sketch/SketchSolver.h"

namespace cadalytic {

namespace {

constexpr double pi = 3.14159265358979323846;

std::string failureMessage()
{
    // Best-effort extraction of the underlying OCCT message.
    try {
        throw;
    } catch (const Standard_Failure& failure) {
        const Standard_CString message = failure.GetMessageString();
        return message == nullptr ? "OCCT operation failed" : std::string(message);
    } catch (...) {
        return "OCCT operation failed";
    }
}

} // namespace

// ---------------------------------------------------------------------------

bool FeatureSolver::findFeature(const Part& part, std::uint64_t featureId,
                                Feature*& outFeature, std::string& error) const
{
    for (const auto& featurePtr : part.features()) {
        if (featurePtr->id() == featureId) {
            outFeature = featurePtr.get();
            return true;
        }
    }
    error = "feature " + std::to_string(featureId) + " not found in part";
    return false;
}

bool FeatureSolver::isSolidType(FeatureType type) const
{
    switch (type) {
        case FeatureType::Solid:
        case FeatureType::Box:
        case FeatureType::Cylinder:
        case FeatureType::Sphere:
        case FeatureType::Cone:
        case FeatureType::Torus:
        case FeatureType::Extrude:
        case FeatureType::Revolve:
        case FeatureType::Loft:
        case FeatureType::Fillet:
        case FeatureType::Chamfer:
        case FeatureType::Shell:
        case FeatureType::Fuse:
        case FeatureType::Cut:
        case FeatureType::Common:
            return true;
        default:
            return false;
    }
}

bool FeatureSolver::evaluateSketch(const Sketch& sketch, TopoDS_Shape& outShape)
{
    if (sketch.lines().empty()) {
        return true; // nothing to promote yet; not an error
    }

    BRepBuilderAPI_MakeWire wireBuilder;
    for (const auto& linePtr : sketch.lines()) {
        const SketchPoint* start = sketch.findPoint(linePtr->startPointId());
        const SketchPoint* end = sketch.findPoint(linePtr->endPointId());
        if (start == nullptr || end == nullptr) {
            continue;
        }
        wireBuilder.Add(BRepBuilderAPI_MakeEdge(
            gp_Pnt(start->x(), start->y(), 0.0),
            gp_Pnt(end->x(), end->y(), 0.0)).Edge());
    }

    if (!wireBuilder.IsDone()) {
        return false;
    }

    // A planar face is required to extrude/revolve the profile, so the wire
    // must close.
    try {
        const TopoDS_Wire& wire = wireBuilder.Wire();
        BRepBuilderAPI_MakeFace faceBuilder(
            gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), wire);
        if (!faceBuilder.IsDone()) {
            return false;
        }
        outShape = faceBuilder.Shape();
        return true;
    } catch (...) {
        return false;
    }
}

bool FeatureSolver::currentSolid(TopoDS_Shape& outShape) const
{
    const auto it = m_shapes.find(m_lastSolidId);
    if (it != m_shapes.end()) {
        outShape = it->second;
        return true;
    }
    return false;
}

bool FeatureSolver::dependencyShape(const Feature& feature, std::size_t index,
                                    TopoDS_Shape& outShape,
                                    std::string& error) const
{
    if (feature.dependencies().size() <= index) {
        error = "feature requires a solid dependency";
        return false;
    }
    const auto it = m_shapes.find(feature.dependencies()[index]);
    if (it == m_shapes.end()) {
        error = "feature dependency shape is not available";
        return false;
    }
    outShape = it->second;
    return true;
}
// ---------------------------------------------------------------------------

FeatureSolveResult FeatureSolver::solve(Part& part)
{
    FeatureSolveResult result{};

    // Dependency graph + dirty propagation (same semantics as RebuildEngine).
    DependencyGraph graph;
    std::set<std::uint64_t> affected;
    for (const auto& featurePtr : part.features()) {
        graph.addNode(featurePtr->id());
        for (const auto dependencyId : featurePtr->dependencies()) {
            graph.addDependency(featurePtr->id(), dependencyId);
        }
    }
    for (const auto& featurePtr : part.features()) {
        if (featurePtr->isDirty()) {
            graph.markDirty(featurePtr->id());
        }
    }
    for (const auto dirtyId : graph.dirtyNodes()) {
        affected.insert(dirtyId);
    }

    const auto order = graph.rebuildOrder();
    if (order.empty() && !part.features().empty()) {
        result.error = graph.error();
        return result;
    }

    for (const auto featureId : order) {
        if (affected.find(featureId) == affected.end()) {
            continue; // clean sub-tree: its shape is already in m_shapes
        }

        Feature* feature = nullptr;
        std::string lookupError;
        if (!findFeature(part, featureId, feature, lookupError)) {
            result.error = lookupError;
            return result;
        }

        std::string evalError;
        if (!evaluate(part, *feature, evalError)) {
            result.error = "feature '" + feature->name() + "' failed: " + evalError;
            return result;
        }

        feature->clearDirty();
        result.rebuiltFeatureIds.push_back(featureId);
    }

    result.success = true;
    return result;
}

bool FeatureSolver::evaluate(Part& part, Feature& feature, std::string& error)
{
    try {
        TopoDS_Shape outShape;

        switch (feature.type()) {
            case FeatureType::Sketch: {
                auto& sketch = static_cast<Sketch&>(feature);
                SketchSolver sketchSolver;
                const SolveResult solved = sketchSolver.solve(sketch);
                if (!solved.success) {
                    error = "sketch solve failed: " + solved.error;
                    return false;
                }
                if (!evaluateSketch(sketch, outShape)) {
                    error = "sketch profile must form a closed wire to create a face";
                    return false;
                }
                break;
            }

            case FeatureType::Box: {
                const double w = feature.param("width", 10.0);
                const double d = feature.param("depth", 10.0);
                const double h = feature.param("height", 10.0);
                outShape = BRepPrimAPI_MakeBox(w, d, h).Shape();
                break;
            }

            case FeatureType::Cylinder: {
                const double r = feature.param("radius", 5.0);
                const double h = feature.param("height", 10.0);
                outShape = BRepPrimAPI_MakeCylinder(r, h).Shape();
                break;
            }

            case FeatureType::Sphere: {
                const double r = feature.param("radius", 5.0);
                outShape = BRepPrimAPI_MakeSphere(r).Shape();
                break;
            }

            case FeatureType::Cone: {
                const double r1 = feature.param("radius1", 5.0);
                const double r2 = feature.param("radius2", 0.0);
                const double h = feature.param("height", 10.0);
                outShape = BRepPrimAPI_MakeCone(r1, r2, h).Shape();
                break;
            }

            case FeatureType::Torus: {
                const double rMajor = feature.param("majorRadius", 10.0);
                const double rMinor = feature.param("minorRadius", 2.0);
                outShape = BRepPrimAPI_MakeTorus(rMajor, rMinor).Shape();
                break;
            }

            case FeatureType::Extrude: {
                if (feature.dependencies().empty()) {
                    error = "extrude requires a profile sketch dependency";
                    return false;
                }
                const auto profileIt = m_shapes.find(feature.dependencies()[0]);
                if (profileIt == m_shapes.end()) {
                    error = "extrude profile shape is not available";
                    return false;
                }
                TopoDS_Face profile;
                TopExp_Explorer faceIt(profileIt->second, TopAbs_FACE);
                if (!faceIt.More()) {
                    error = "extrude profile did not produce a planar face";
                    return false;
                }
                profile = TopoDS::Face(faceIt.Current());
                const double depth = feature.param("depth", 10.0);
                outShape = BRepPrimAPI_MakePrism(
                    profile, gp_Vec(0.0, 0.0, depth)).Shape();
                break;
            }

            case FeatureType::Revolve: {
                if (feature.dependencies().empty()) {
                    error = "revolve requires a profile sketch dependency";
                    return false;
                }
                const auto profileIt = m_shapes.find(feature.dependencies()[0]);
                if (profileIt == m_shapes.end()) {
                    error = "revolve profile shape is not available";
                    return false;
                }
                TopoDS_Face profile;
                TopExp_Explorer faceIt(profileIt->second, TopAbs_FACE);
                if (!faceIt.More()) {
                    error = "revolve profile did not produce a planar face";
                    return false;
                }
                profile = TopoDS::Face(faceIt.Current());
                const double angleDeg = feature.param("angle", 360.0);
                outShape = BRepPrimAPI_MakeRevol(
                    profile,
                    gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)),
                    angleDeg * pi / 180.0).Shape();
                break;
            }

            case FeatureType::Loft: {
                if (feature.dependencies().size() < 2) {
                    error = "loft requires at least two profile sketches";
                    return false;
                }
                BRepOffsetAPI_ThruSections loft(Standard_True, Standard_True, 1e-6);
                for (const auto depId : feature.dependencies()) {
                    const auto wireIt = m_shapes.find(depId);
                    if (wireIt == m_shapes.end()) {
                        error = "loft profile shape is not available";
                        return false;
                    }
                    TopExp_Explorer wireExp(wireIt->second, TopAbs_WIRE);
                    if (!wireExp.More()) {
                        error = "loft profile contains no wire";
                        return false;
                    }
                    loft.AddWire(TopoDS::Wire(wireExp.Current()));
                }
                loft.Build();
                if (!loft.IsDone()) {
                    error = "loft construction failed";
                    return false;
                }
                outShape = loft.Shape();
                break;
            }

            case FeatureType::Fillet: {
                TopoDS_Shape base;
                if (!dependencyShape(feature, 0, base, error)) {
                    return false;
                }
                const double radius = feature.param("radius", 1.0);
                BRepFilletAPI_MakeFillet fillet(base);
                for (TopExp_Explorer edgeIt(base, TopAbs_EDGE); edgeIt.More(); edgeIt.Next()) {
                    fillet.Add(radius, TopoDS::Edge(edgeIt.Current()));
                }
                if (!fillet.IsDone()) {
                    error = "fillet construction failed";
                    return false;
                }
                outShape = fillet.Shape();
                break;
            }

            case FeatureType::Chamfer: {
                TopoDS_Shape base;
                if (!dependencyShape(feature, 0, base, error)) {
                    return false;
                }
                const double distance = feature.param("distance", 1.0);
                BRepFilletAPI_MakeChamfer chamfer(base);
                for (TopExp_Explorer edgeIt(base, TopAbs_EDGE); edgeIt.More(); edgeIt.Next()) {
                    chamfer.Add(distance, TopoDS::Edge(edgeIt.Current()));
                }
                if (!chamfer.IsDone()) {
                    error = "chamfer construction failed";
                    return false;
                }
                outShape = chamfer.Shape();
                break;
            }

            case FeatureType::Shell: {
                TopoDS_Shape base;
                if (!dependencyShape(feature, 0, base, error)) {
                    return false;
                }
                const double thickness = feature.param("thickness", 1.0);
                // Remove the first face of the base solid to open the shell.
                TopTools_ListOfShape openFaces;
                TopExp_Explorer faceIt(base, TopAbs_FACE);
                if (faceIt.More()) {
                    openFaces.Append(faceIt.Current());
                } else {
                    error = "shell base solid contains no faces";
                    return false;
                }
                BRepOffsetAPI_MakeThickSolid shell;
                shell.MakeThickSolidByJoin(base, openFaces, thickness, 1e-3);
                shell.Build();
                if (!shell.IsDone()) {
                    error = "shell construction failed";
                    return false;
                }
                outShape = shell.Shape();
                break;
            }

case FeatureType::Fuse:
            case FeatureType::Cut:
            case FeatureType::Common: {
                if (feature.dependencies().size() < 2) {
                    error = "boolean operation requires two operand features";
                    return false;
                }
                const auto op1It = m_shapes.find(feature.dependencies()[0]);
                const auto op2It = m_shapes.find(feature.dependencies()[1]);
                if (op1It == m_shapes.end() || op2It == m_shapes.end()) {
                    error = "boolean operand shape is not available";
                    return false;
                }
                switch (feature.type()) {
                    case FeatureType::Fuse:
                        outShape = BRepAlgoAPI_Fuse(op1It->second, op2It->second).Shape();
                        break;
                    case FeatureType::Cut:
                        outShape = BRepAlgoAPI_Cut(op1It->second, op2It->second).Shape();
                        break;
                    case FeatureType::Common:
                        outShape = BRepAlgoAPI_Common(op1It->second, op2It->second).Shape();
                        break;
                    default:
                        error = "unsupported boolean operation";
                        return false;
                }
                break;
            }

            default:
                // Generic/Solid: nothing to compute yet.
                return true;
        }

        // Optional placement: translate the freshly evaluated geometry.
        const double tx = feature.param("tx", 0.0);
        const double ty = feature.param("ty", 0.0);
        const double tz = feature.param("tz", 0.0);
        if (tx != 0.0 || ty != 0.0 || tz != 0.0) {
            gp_Trsf trsf;
            trsf.SetTranslation(gp_Vec(tx, ty, tz));
            outShape = BRepBuilderAPI_Transform(outShape, trsf, Standard_True).Shape();
        }

        m_shapes[feature.id()] = outShape;
        if (isSolidType(feature.type())) {
            m_lastSolidId = feature.id();
        }
        return true;

    } catch (...) {
        error = failureMessage();
        return false;
    }
}

} // namespace cadalytic