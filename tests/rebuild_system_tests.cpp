#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "document/Document.h"
#include "rebuild/RebuildSafeReferences.h"
#include "rebuild/TopologicalNaming.h"

namespace {

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

// ---------------------------------------------------------------------------
// Test fixture: a rectangular box built from synthetic element facts. The
// "+X" face offset is parameterisable so tests can simulate a face that moved.
// ---------------------------------------------------------------------------

struct BoxTopology
{
    std::vector<cadalytic::TopoElementFact> facts;
    std::size_t faceCount;
    std::size_t edgeCount;
    std::size_t vertexCount;
};

BoxTopology makeBox(double rightOffset)
{
    const std::string dx = std::to_string(rightOffset);
    const std::string f1 = "plane(+z,d=5)";
    const std::string f2 = "plane(-z,d=5)";
    const std::string f3 = "plane(+y,d=5)";
    const std::string f4 = "plane(-y,d=5)";
    const std::string f5 = "plane(+x,d=" + dx + ")";
    const std::string f6 = "plane(-x,d=5)";

    const std::vector<std::string> e{
        "edge_e1", "edge_e2", "edge_e3", "edge_e4",
        "edge_e5", "edge_e6", "edge_e7", "edge_e8",
        "edge_e9", "edge_e10", "edge_e11", "edge_e12"};
    const std::vector<std::string> v{
        "vertex_1", "vertex_2", "vertex_3", "vertex_4",
        "vertex_5", "vertex_6", "vertex_7", "vertex_8"};

    BoxTopology box;
    box.faceCount = 6;
    box.edgeCount = 12;
    box.vertexCount = 8;

    auto& facts = box.facts;
    facts.push_back({cadalytic::TopoElementType::Face, "Plane", f1, "Top",
                     {e[0], e[1], e[2], e[3]}});
    facts.push_back({cadalytic::TopoElementType::Face, "Plane", f2, "Bottom",
                     {e[4], e[5], e[6], e[7]}});
    facts.push_back({cadalytic::TopoElementType::Face, "Plane", f3, "Front",
                     {e[1], e[6], e[8], e[9]}});
    facts.push_back({cadalytic::TopoElementType::Face, "Plane", f4, "Back",
                     {e[3], e[4], e[10], e[11]}});
    facts.push_back({cadalytic::TopoElementType::Face, "Plane", f5, "Right",
                     {e[0], e[5], e[8], e[10]}});
    facts.push_back({cadalytic::TopoElementType::Face, "Plane", f6, "Left",
                     {e[2], e[7], e[9], e[11]}});

    facts.push_back({cadalytic::TopoElementType::Edge, "Line", e[0], "",
                     {f1, f5, v[5], v[6]}});
    facts.push_back({cadalytic::TopoElementType::Edge, "Line", e[1], "",
                     {f1, f3, v[6], v[7]}});
    facts.push_back({cadalytic::TopoElementType::Edge, "Line", e[2], "",
                     {f1, f6, v[4], v[7]}});
    facts.push_back({cadalytic::TopoElementType::Edge, "Line", e[3], "",
                     {f1, f4, v[4], v[5]}});
    facts.push_back({cadalytic::TopoElementType::Edge, "Line", e[4], "",
                     {f2, f4, v[0], v[1]}});
    facts.push_back({cadalytic::TopoElementType::Edge, "Line", e[5], "",
                     {f2, f5, v[1], v[2]}});
    facts.push_back({cadalytic::TopoElementType::Edge, "Line", e[6], "",
                     {f2, f3, v[2], v[3]}});
    facts.push_back({cadalytic::TopoElementType::Edge, "Line", e[7], "",
                     {f2, f6, v[0], v[3]}});
    facts.push_back({cadalytic::TopoElementType::Edge, "Line", e[8], "",
                     {f3, f5, v[2], v[6]}});
    facts.push_back({cadalytic::TopoElementType::Edge, "Line", e[9], "",
                     {f3, f6, v[3], v[7]}});
    facts.push_back({cadalytic::TopoElementType::Edge, "Line", e[10], "",
                     {f4, f5, v[1], v[5]}});
    facts.push_back({cadalytic::TopoElementType::Edge, "Line", e[11], "",
                     {f4, f6, v[0], v[4]}});

    facts.push_back({cadalytic::TopoElementType::Vertex, "Point", v[0], "",
                     {f2, f4, f6, e[4], e[7], e[11]}});
    facts.push_back({cadalytic::TopoElementType::Vertex, "Point", v[1], "",
                     {f2, f4, f5, e[4], e[5], e[10]}});
    facts.push_back({cadalytic::TopoElementType::Vertex, "Point", v[2], "",
                     {f2, f3, f5, e[5], e[6], e[8]}});
    facts.push_back({cadalytic::TopoElementType::Vertex, "Point", v[3], "",
                     {f2, f3, f6, e[6], e[7], e[9]}});
    facts.push_back({cadalytic::TopoElementType::Vertex, "Point", v[4], "",
                     {f1, f4, f6, e[2], e[3], e[11]}});
    facts.push_back({cadalytic::TopoElementType::Vertex, "Point", v[5], "",
                     {f1, f4, f5, e[0], e[3], e[10]}});
    facts.push_back({cadalytic::TopoElementType::Vertex, "Point", v[6], "",
                     {f1, f3, f5, e[0], e[1], e[8]}});
    facts.push_back({cadalytic::TopoElementType::Vertex, "Point", v[7], "",
                     {f1, f3, f6, e[1], e[2], e[9]}});

    return box;
}

// Same box but the "+X" face is removed entirely.
BoxTopology makeBoxWithoutRightFace()
{
    const std::string f1 = "plane(+z,d=5)";
    const std::string f2 = "plane(-z,d=5)";
    const std::string f3 = "plane(+y,d=5)";
    const std::string f4 = "plane(-y,d=5)";
    const std::string f6 = "plane(-x,d=5)";

    const std::vector<std::string> e{
        "edge_e1", "edge_e2", "edge_e3", "edge_e4",
        "edge_e5", "edge_e6", "edge_e7", "edge_e8",
        "edge_e9", "edge_e10", "edge_e11", "edge_e12"};

    BoxTopology box;
    box.faceCount = 5;
    box.edgeCount = 12;
    box.vertexCount = 8;

    auto& facts = box.facts;
    facts.push_back({cadalytic::TopoElementType::Face, "Plane", f1, "Top",
                     {e[0], e[1], e[2], e[3]}});
    facts.push_back({cadalytic::TopoElementType::Face, "Plane", f2, "Bottom",
                     {e[4], e[5], e[6], e[7]}});
    facts.push_back({cadalytic::TopoElementType::Face, "Plane", f3, "Front",
                     {e[1], e[6], e[8], e[9]}});
    facts.push_back({cadalytic::TopoElementType::Face, "Plane", f4, "Back",
                     {e[3], e[4], e[10], e[11]}});
    facts.push_back({cadalytic::TopoElementType::Face, "Plane", f6, "Left",
                     {e[2], e[7], e[9], e[11]}});
    return box;
}

void testFirstPassAssignsStableNames()
{
    cadalytic::TopologicalNaming naming;
    const auto names = naming.nameElements(1, makeBox(5.0).facts);

    require(names.size() == 26, "box should produce 26 named elements");
    require(names[0].name() == "Face1", "first face should be Face1");
    require(names[5].name() == "Face6", "sixth face should be Face6");
    require(names[6].name() == "Edge1", "first edge should be Edge1");
    require(names[17].name() == "Edge12", "last edge should be Edge12");
    require(names[18].name() == "Vertex1", "first vertex should be Vertex1");
    require(names[25].name() == "Vertex8", "last vertex should be Vertex8");

    const auto* top = naming.findByTag(1, cadalytic::TopoElementType::Face,
                                       cadalytic::SemanticTag::Top);
    require(top != nullptr && top->name() == "Face1", "top face should be tagged and named Face1");
    require(naming.findByTag(1, cadalytic::TopoElementType::Face,
                             cadalytic::SemanticTag::Right)->name() == "Face5",
            "right face should resolve via its semantic tag");
}

void testIdenticalRebuildKeepsNames()
{
    cadalytic::TopologicalNaming naming;
    const auto first = naming.nameElements(2, makeBox(5.0).facts);
    const auto second = naming.nameElements(2, makeBox(5.0).facts);

    for (std::size_t i = 0; i < first.size(); ++i) {
        require(second[i].name() == first[i].name(),
                "identical rebuild should reuse every name");
    }
    require(naming.find(2, cadalytic::TopoElementType::Face, "Face3") != nullptr,
            "Face3 should resolve as active after identical rebuild");
}

void testReorderKeepsNamesBySignature()
{
    cadalytic::TopologicalNaming naming;
    naming.nameElements(3, makeBox(5.0).facts);

    std::vector<cadalytic::TopoElementFact> reordered = makeBox(5.0).facts;
    std::swap(reordered[0], reordered[1]); // Top and Bottom faces swap order

    const auto names = naming.nameElements(3, reordered);
    require(names[0].name() == "Face2", "bottom face fact should still be Face2 when first");
    require(names[1].name() == "Face1", "top face fact should still be Face1 when second");
    require(names[6].name() == "Edge1", "edges should remain stable after reorder");
}

void testInsertionKeepsExistingNames()
{
    cadalytic::TopologicalNaming naming;
    naming.nameElements(4, makeBox(5.0).facts);

    std::vector<cadalytic::TopoElementFact> extended = makeBox(5.0).facts;
    extended.insert(
        extended.begin() + 3,
        cadalytic::TopoElementFact{cadalytic::TopoElementType::Face, "Plane",
                                   "plane(+x2,d=5)", "", {"edge_e2", "edge_e7"}});

    const auto names = naming.nameElements(4, extended);
    require(names[3].name() == "Face7", "inserted face should get the next free index");
    require(names[4].name() == "Face4", "face after the inserted one keeps its name");
    require(names[5].name() == "Face5", "face after the inserted one keeps its name");
}

void testDuplicateSignaturesReceiveDistinctNames()
{
    cadalytic::TopologicalNaming naming;
    const std::vector<cadalytic::TopoElementFact> first{
        {cadalytic::TopoElementType::Edge, "Line", "line(x=1)", "", {"face_a"}},
        {cadalytic::TopoElementType::Edge, "Line", "line(x=1)", "", {"face_b"}}};
    const auto initial = naming.nameElements(9, first);
    require(initial[0].name() != initial[1].name(),
            "duplicate signatures should receive distinct names");

    const auto reordered = naming.nameElements(9, std::vector<cadalytic::TopoElementFact>{
        first[1], first[0]});
    require(reordered[0].name() == initial[1].name()
                && reordered[1].name() == initial[0].name(),
            "duplicate signatures should remain distinct after reorder");
}

void testAdjacencyFallbackForMovedFace()
{
    cadalytic::TopologicalNaming naming;
    const auto original = naming.nameElements(5, makeBox(5.0).facts);
    require(original[4].name() == "Face5", "right face should initially be Face5");

    // The "+X" face moves outward; its signature changes but its boundary
    // (adjacency) does not. The name must be recovered via adjacency.
    const auto moved = naming.nameElements(5, makeBox(8.0).facts);
    require(moved[4].name() == "Face5",
            "moved face should keep its name via adjacency matching");
    require(moved[4].signature() == "plane(+x,d=8.000000)",
            "moved face should carry the new signature");

    require(naming.find(5, cadalytic::TopoElementType::Face, "Face1") != nullptr,
            "unchanged faces should still resolve normally");
}

void testPartialAdjacencyFallback()
{
    cadalytic::TopologicalNaming naming;
    const std::vector<cadalytic::TopoElementFact> first{
        {cadalytic::TopoElementType::Face, "Plane", "plane(a)", "", {"edge_a", "edge_b",
                                                                         "edge_c", "edge_d"}}};
    naming.nameElements(10, first);

    const std::vector<cadalytic::TopoElementFact> moved{
        {cadalytic::TopoElementType::Face, "Plane", "plane(b)", "", {"edge_a", "edge_b",
                                                                         "edge_x", "edge_y"}}};
    const auto result = naming.nameElements(10, moved);
    require(result[0].name() == "Face1",
            "a face with half of its adjacency preserved should retain its name");
}

void testDeletedFaceDeactivatesAndReactivates()
{
    cadalytic::TopologicalNaming naming;
    naming.nameElements(6, makeBox(5.0).facts);
    require(naming.find(6, cadalytic::TopoElementType::Face, "Face5") != nullptr,
            "right face should resolve before deletion");

    // Geometry element disappears completely.
    naming.nameElements(6, makeBoxWithoutRightFace().facts);
    require(naming.find(6, cadalytic::TopoElementType::Face, "Face5") == nullptr,
            "deleted face should no longer resolve");
    require(naming.findBySignature(6, cadalytic::TopoElementType::Face,
                                   "plane(+x,d=5.000000)") == nullptr,
            "deleted face signature should no longer resolve");

    // The element returns on a later rebuild; the old name is restored.
    naming.nameElements(6, makeBox(5.0).facts);
    const auto* restored = naming.find(6, cadalytic::TopoElementType::Face, "Face5");
    require(restored != nullptr, "restored face should resolve again");
    require(restored->active(), "restored face should be active");
}

void testFeatureReferenceLifecycle()
{
    cadalytic::Document document("Feature reference test");
    cadalytic::Part& part = document.addPart("Part");
    const auto& feature = part.addFeature("Base");
    cadalytic::TopologicalNaming naming;

    cadalytic::RebuildSafeReferences references;
    const auto liveId = references.addFeatureReference(feature.id());
    const auto missingId = references.addFeatureReference(9999);

    require(references.contains(liveId) && references.contains(missingId),
            "references should be registered");
    const auto dead = references.validate(part, naming);
    require(dead.size() == 1 && dead[0] == missingId,
            "only the reference to a missing feature should die");

    require(references.remove(missingId), "removal should succeed");
    require(!references.contains(missingId), "removed reference should be gone");
    require(references.validate(part, naming).empty(),
            "remaining reference should stay alive");
}
void testElementReferenceSurvivesMoveButGeometryReferenceDoesNot()
{
    cadalytic::Document document("Reference move test");
    cadalytic::Part& part = document.addPart("Part");
    cadalytic::TopologicalNaming naming;

    const std::uint64_t featureId = 7;
    naming.nameElements(featureId, makeBox(5.0).facts);

    cadalytic::RebuildSafeReferences references;
    const auto byName = references.addElementReference(
        featureId, cadalytic::TopoElementType::Face, "Face5");
    const auto byGeometry = references.addGeometryReference(
        featureId, cadalytic::TopoElementType::Face, "plane(+x,d=5.000000)");

    require(references.validate(part, naming).empty(),
            "both references should resolve before the face moves");

    // The "+X" face moves; its name survives but its signature does not.
    naming.nameElements(featureId, makeBox(8.0).facts);
    const auto dead = references.validate(part, naming);
    require(dead.size() == 1 && dead[0] == byGeometry,
            "only the signature-based reference should die when the face moves");
    require(std::find(dead.begin(), dead.end(), byName) == dead.end(),
            "the name-based reference should survive the move");
    require(references.all()[0].alive() && !references.all()[1].alive(),
            "name-based reference alive, signature-based reference dead");
}

void testDeletionKillsElementReferenceUntilElementReturns()
{
    cadalytic::Document document("Reference deletion test");
    cadalytic::Part& part = document.addPart("Part");
    cadalytic::TopologicalNaming naming;

    const std::uint64_t featureId = 8;
    naming.nameElements(featureId, makeBox(5.0).facts);

    cadalytic::RebuildSafeReferences references;
    const auto topRef = references.addElementReference(
        featureId, cadalytic::TopoElementType::Face, "Face1");
    const auto rightRef = references.addElementReference(
        featureId, cadalytic::TopoElementType::Face, "Face5");

    require(references.validate(part, naming).empty(),
            "both element references should resolve initially");

    // The right face's geometry element is deleted.
    naming.nameElements(featureId, makeBoxWithoutRightFace().facts);
    const auto dead = references.validate(part, naming);
    require(dead.size() == 1 && dead[0] == rightRef,
            "only the reference to the deleted face should die");
    require(std::find(dead.begin(), dead.end(), topRef) == dead.end(),
            "the reference to the surviving face must stay alive");

    // The face returns; its reference becomes alive again without re-adding.
    naming.nameElements(featureId, makeBox(5.0).facts);
    require(references.validate(part, naming).empty(),
            "element reference should revive when the face returns");
}

} // namespace

int main()
{
    testFirstPassAssignsStableNames();
    testIdenticalRebuildKeepsNames();
    testReorderKeepsNamesBySignature();
    testInsertionKeepsExistingNames();
    testDuplicateSignaturesReceiveDistinctNames();
    testAdjacencyFallbackForMovedFace();
    testPartialAdjacencyFallback();
    testDeletedFaceDeactivatesAndReactivates();
    testFeatureReferenceLifecycle();
    testElementReferenceSurvivesMoveButGeometryReferenceDoesNot();
    testDeletionKillsElementReferenceUntilElementReturns();
    std::cout << "All rebuild system tests passed.\n";
    return EXIT_SUCCESS;
}