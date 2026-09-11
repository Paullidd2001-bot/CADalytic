#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "document/Document.h"
#include "feature/Feature.h"
#include "sketch/Sketch.h"
#include "DocumentSerializer.h"
#include "FileFormat.h"
#include "compression/Compression.h"

namespace {

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

cadalytic::Document makeFixture()
{
    cadalytic::Document doc("Fixture");
    auto& part = doc.addPart("Part 1");

    auto& sketch = part.addSketch("Profile");
    auto& p1 = sketch.addPoint(0.0, 0.0, true);
    auto& p2 = sketch.addPoint(10.0, 0.0);
    auto& p3 = sketch.addPoint(10.0, 20.0);
    auto& p4 = sketch.addPoint(0.0, 20.0);
    sketch.addLine(p1.id(), p2.id());
    sketch.addLine(p2.id(), p3.id());
    sketch.addLine(p3.id(), p4.id());
    sketch.addLine(p4.id(), p1.id());
    sketch.addConstraint(cadalytic::ConstraintType::Distance, 10.0,
                         {p2.id(), p3.id()});

    auto& extrude = part.addFeature("Pad Feature", cadalytic::FeatureType::Extrude);
    extrude.addDependency(sketch.id());
    extrude.setParam("depth", 5.0);
    extrude.setParam("tx", 2.0);
    return doc;
}

void testTextRoundTripPreservesEverything()
{
    const cadalytic::Document doc = makeFixture();
    const std::string text = cadalytic::serialization::serialize(doc);

    std::string error;
    const auto loaded = cadalytic::serialization::deserialize(text, error);
    require(loaded != nullptr, "deserialize failed: " + error);
    require(loaded->name() == "Fixture", "document name should round-trip");
    require(loaded->parts().size() == 1, "part count should round-trip");

    const auto& part = *loaded->parts().front();
    require(part.name() == "Part 1", "part name should round-trip");
    require(part.features().size() == 2, "feature count should round-trip");

    const auto& extrude = *part.features().back();
    require(extrude.name() == "Pad Feature",
            "feature name should round-trip");
    require(extrude.type() == cadalytic::FeatureType::Extrude,
            "feature type should round-trip");
    require(extrude.hasParam("depth") && extrude.param("depth") == 5.0,
            "double param should round-trip");
    require(extrude.hasParam("tx") && extrude.param("tx") == 2.0,
            "placement param should round-trip");
    require(extrude.dependencies().size() == 1,
            "dependency should round-trip");

    const auto* sketch = dynamic_cast<const cadalytic::Sketch*>(
        part.features().front().get());
    require(sketch != nullptr, "sketch should round-trip as Sketch");
    require(sketch->type() == cadalytic::FeatureType::Sketch,
            "sketch type should round-trip");
    require(sketch->points().size() == 4, "points should round-trip");
    require(sketch->lines().size() == 4, "lines should round-trip");
    require(sketch->constraints().size() == 1, "constraints should round-trip");
}

void testBadHeaderRejected()
{
    std::string error;
    const auto loaded =
        cadalytic::serialization::deserialize("NOT_A_DOC v99\nend\n", error);
    require(loaded == nullptr, "bad header should be rejected");
    require(!error.empty(), "error message should be populated");
}

void testFileFormatPlainAndCompressed()
{
    const cadalytic::Document doc = makeFixture();

    const std::string plain = cadalytic::serialization::toFileText(doc, false);
    std::string error;

    const auto fromPlain =
        cadalytic::serialization::fromFileText(plain, error);
    require(fromPlain != nullptr, "plain envelope should load: " + error);
    require(fromPlain->parts().size() == 1, "plain envelope part count");

    const std::string packed = cadalytic::serialization::toFileText(doc, true);
    const auto fromPacked =
        cadalytic::serialization::fromFileText(packed, error);
    require(fromPacked != nullptr, "compressed envelope should load: " + error);
    require(fromPacked->parts().size() == 1, "compressed envelope part count");
}

void testFileFormatRejectsGarbage()
{
    std::string error;
    require(cadalytic::serialization::fromFileText("hello world", error) == nullptr,
            "garbage should be rejected");
}

void testCompressionRoundTrip()
{
    const std::string original =
        "width 10.0\nheight 20.0\nwidth 10.0\nheight 20.0\n"
        "radius 5 radius 5 radius 5 depth 3 depth 3 depth 3\n";
    const std::string packed = cadalytic::serialization::compression::compress(original);
    std::string error;
    std::string restored;
    require(cadalytic::serialization::compression::decompress(packed, restored, error),
            "decompression should succeed: " + error);
    require(restored == original, "compression should round-trip exactly");
}

void testFileSaveLoadDisk()
{
    const cadalytic::Document doc = makeFixture();
    const std::string path = "serialization_test_tmp.cady";

    require(cadalytic::serialization::save(doc, path),
            "save() should write the file");
    std::string error;
    const auto loaded = cadalytic::serialization::load(path, error);
    require(loaded != nullptr, "load() should read the file: " + error);
    require(loaded->name() == "Fixture", "loaded document should match");

    std::remove(path.c_str());
}

} // namespace

int main()
{
    testTextRoundTripPreservesEverything();
    testBadHeaderRejected();
    testFileFormatPlainAndCompressed();
    testFileFormatRejectsGarbage();
    testCompressionRoundTrip();
    testFileSaveLoadDisk();
    std::cout << "All serialization tests passed.\n";
    return EXIT_SUCCESS;
}