#include "DocumentSerializer.h"

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "sketch/Sketch.h"
#include "sketch/SketchPoint.h"
#include "sketch/SketchLine.h"
#include "constraint/Constraint.h"
#include "feature/Feature.h"

namespace cadalytic {
namespace serialization {

namespace {

std::string featureTypeToString(FeatureType type)
{
    switch (type) {
        case FeatureType::Sketch:  return "SKETCH";
        case FeatureType::Solid:   return "SOLID";
        case FeatureType::Generic: return "GENERIC";
        case FeatureType::Box:     return "BOX";
        case FeatureType::Cylinder: return "CYLINDER";
        case FeatureType::Sphere:  return "SPHERE";
        case FeatureType::Cone:    return "CONE";
        case FeatureType::Torus:   return "TORUS";
        case FeatureType::Extrude: return "EXTRUDE";
        case FeatureType::Revolve: return "REVOLVE";
        case FeatureType::Loft:    return "LOFT";
        case FeatureType::Fillet:  return "FILLET";
        case FeatureType::Chamfer: return "CHAMFER";
        case FeatureType::Shell:   return "SHELL";
        case FeatureType::Fuse:    return "FUSE";
        case FeatureType::Cut:     return "CUT";
        case FeatureType::Common:  return "COMMON";
    }
    return "GENERIC";
}

FeatureType stringToFeatureType(const std::string& s)
{
    if (s == "SKETCH")   return FeatureType::Sketch;
    if (s == "SOLID")    return FeatureType::Solid;
    if (s == "GENERIC")  return FeatureType::Generic;
    if (s == "BOX")      return FeatureType::Box;
    if (s == "CYLINDER") return FeatureType::Cylinder;
    if (s == "SPHERE")   return FeatureType::Sphere;
    if (s == "CONE")     return FeatureType::Cone;
    if (s == "TORUS")    return FeatureType::Torus;
    if (s == "EXTRUDE")  return FeatureType::Extrude;
    if (s == "REVOLVE")  return FeatureType::Revolve;
    if (s == "LOFT")     return FeatureType::Loft;
    if (s == "FILLET")   return FeatureType::Fillet;
    if (s == "CHAMFER")  return FeatureType::Chamfer;
    if (s == "SHELL")    return FeatureType::Shell;
    if (s == "FUSE")     return FeatureType::Fuse;
    if (s == "CUT")      return FeatureType::Cut;
    if (s == "COMMON")   return FeatureType::Common;
    return FeatureType::Generic;
}

// True when the token names a serializable feature type.
bool looksLikeFeatureType(const std::string& s)
{
    if (s == "SKETCH" || s == "SOLID" || s == "GENERIC" ||
        s == "BOX" || s == "CYLINDER" || s == "SPHERE" ||
        s == "CONE" || s == "TORUS" || s == "EXTRUDE" ||
        s == "REVOLVE" || s == "LOFT" || s == "FILLET" ||
        s == "CHAMFER" || s == "SHELL" || s == "FUSE" ||
        s == "CUT" || s == "COMMON") {
        return true;
    }
    return false;
}

std::string constraintTypeToString(ConstraintType type)
{
    switch (type) {
        case ConstraintType::Coincident:    return "COINCIDENT";
        case ConstraintType::Horizontal:    return "HORIZONTAL";
        case ConstraintType::Vertical:      return "VERTICAL";
        case ConstraintType::Distance:      return "DISTANCE";
        case ConstraintType::Angle:         return "ANGLE";
    }
    return "UNKNOWN";
}

ConstraintType stringToConstraintType(const std::string& s)
{
    if (s == "COINCIDENT") return ConstraintType::Coincident;
    if (s == "HORIZONTAL") return ConstraintType::Horizontal;
    if (s == "VERTICAL")   return ConstraintType::Vertical;
    if (s == "DISTANCE")   return ConstraintType::Distance;
    if (s == "ANGLE")      return ConstraintType::Angle;
    return ConstraintType::Distance;
}

std::string trim(const std::string& s)
{
    const auto begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) return "";
    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

std::vector<std::string> split(const std::string& line)
{
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string tok;
    while (iss >> tok) tokens.push_back(tok);
    return tokens;
}

std::string joinTokens(const std::vector<std::string>& tokens,
                       std::size_t first)
{
    std::string result;
    for (std::size_t i = first; i < tokens.size(); ++i) {
        if (!result.empty()) result += ' ';
        result += tokens[i];
    }
    return result;
}

} // namespace

// ---- serialize ----

std::string serialize(const Document& doc)
{
    std::ostringstream out;
    out << "CADALYTIC_DOC v1\n";
    out << "name " << doc.name() << "\n";

    for (const auto& partPtr : doc.parts()) {
        const Part& part = *partPtr;
        out << "part " << part.id() << " " << part.name() << "\n";
        for (const auto& featPtr : part.features()) {
            const Feature& feat = *featPtr;
            out << "  " << featureTypeToString(feat.type())
                << " " << feat.id() << " " << feat.name() << "\n";

            if (feat.type() == FeatureType::Sketch) {
                const auto* sketch = dynamic_cast<const Sketch*>(&feat);
                if (sketch) {
                    for (const auto& pt : sketch->points()) {
                        out << "    point " << pt->id() << " "
                            << pt->x() << " " << pt->y() << " "
                            << (pt->fixed() ? "1" : "0") << "\n";
                    }
                    for (const auto& ln : sketch->lines()) {
                        out << "    line " << ln->id() << " "
                            << ln->startPointId() << " "
                            << ln->endPointId() << "\n";
                    }
                    for (const auto& con : sketch->constraints()) {
                        out << "    constraint " << con->id() << " "
                            << constraintTypeToString(con->type())
                            << " " << con->value();
                        for (std::uint64_t pid : con->pointIds())
                            out << " p" << pid;
                        for (std::uint64_t lid : con->lineIds())
                            out << " l" << lid;
                        out << "\n";
                    }
                }
            }

            for (const auto& param : feat.params())
                out << "    param " << param.first << " " << param.second << "\n";

            for (std::uint64_t depId : feat.dependencies())
                out << "    dependency " << depId << "\n";
        }
        out << "  endpart\n";
    }
    out << "end\n";
    return out.str();
}

// ---- deserialize ----

std::unique_ptr<Document> deserialize(const std::string& text,
                                      std::string& outError)
{
    outError.clear();
    std::istringstream iss(text);
    std::string line;

    if (!std::getline(iss, line)) {
        outError = "empty input";
        return nullptr;
    }
    line = trim(line);
    if (line != "CADALYTIC_DOC v1") {
        outError = "bad header: " + line;
        return nullptr;
    }

    // Document name (may contain spaces).
    std::string docName;
    if (!std::getline(iss, line)) {
        outError = "missing document name";
        return nullptr;
    }
    {
        line = trim(line);
        const std::string prefix = "name ";
        if (line.rfind(prefix, 0) != 0) {
            outError = "expected 'name': " + line;
            return nullptr;
        }
        docName = line.substr(prefix.size());
    }

    auto doc = std::make_unique<Document>(docName);

    std::unordered_map<std::uint64_t, std::uint64_t> partMap;
    std::unordered_map<std::uint64_t, std::uint64_t> featureMap;
    std::unordered_map<std::uint64_t, std::uint64_t> pointMap;
    std::unordered_map<std::uint64_t, std::uint64_t> lineMap;

    struct PendingConstraint {
        std::uint64_t newId;
        ConstraintType type;
        double value;
        std::vector<std::uint64_t> pointSrcIds;
        std::vector<std::uint64_t> lineSrcIds;
    };
    std::vector<PendingConstraint> pendingConstraints;

    enum ParseState { TopLevel, InPart, InFeature };
    ParseState state = TopLevel;

    Part* currentPart = nullptr;
    Feature* currentFeature = nullptr;
    Sketch* currentSketch = nullptr;

    while (std::getline(iss, line)) {
        line = trim(line);
        if (line.empty()) continue;
        auto tokens = split(line);

        if (state == TopLevel) {
            if (tokens[0] == "part") {
                if (tokens.size() < 3) {
                    outError = "malformed part line: " + line;
                    return nullptr;
                }
                std::uint64_t srcId = std::stoull(tokens[1]);
                currentPart = &doc->addPart(joinTokens(tokens, 2));
                partMap[srcId] = currentPart->id();
                state = InPart;
            } else if (tokens[0] == "end") {
                break;
            } else {
                outError = "unexpected top-level token: " + tokens[0];
                return nullptr;
            }
            continue;
        }

        if (state == InPart) {
            if (looksLikeFeatureType(tokens[0])) {
                if (tokens.size() < 3) {
                    outError = "malformed feature line: " + line;
                    return nullptr;
                }
                std::uint64_t srcId = std::stoull(tokens[1]);
                FeatureType ftype = stringToFeatureType(tokens[0]);
                if (ftype == FeatureType::Sketch) {
                    currentFeature = &currentPart->addSketch(joinTokens(tokens, 2));
                } else {
                    currentFeature = &currentPart->addFeature(
                        joinTokens(tokens, 2), ftype);
                }
                featureMap[srcId] = currentFeature->id();
                currentSketch = (ftype == FeatureType::Sketch)
                                    ? dynamic_cast<Sketch*>(currentFeature)
                                    : nullptr;
                state = InFeature;
            } else if (tokens[0] == "endpart") {
                state = TopLevel;
                currentPart = nullptr;
                currentFeature = nullptr;
                currentSketch = nullptr;
            } else {
                outError = "unexpected part-level token: " + tokens[0];
                return nullptr;
            }
            continue;
        }

        // state == InFeature
        // A feature-type token here signals that the previous feature's
        // sub-field block has ended and a new feature begins in the current
        // part (mirrors the InPart handler). Stay in InFeature so the new
        // feature's param/dependency lines are consumed by the same section.
        if (looksLikeFeatureType(tokens[0])) {
            if (tokens.size() < 3) {
                outError = "malformed feature line: " + line;
                return nullptr;
            }
            std::uint64_t srcId = std::stoull(tokens[1]);
            FeatureType ftype = stringToFeatureType(tokens[0]);
            if (ftype == FeatureType::Sketch) {
                currentFeature = &currentPart->addSketch(joinTokens(tokens, 2));
            } else {
                currentFeature = &currentPart->addFeature(
                    joinTokens(tokens, 2), ftype);
            }
            featureMap[srcId] = currentFeature->id();
            currentSketch = (ftype == FeatureType::Sketch)
                                ? dynamic_cast<Sketch*>(currentFeature)
                                : nullptr;
            continue;
        }
        if (tokens[0] == "point") {
            if (tokens.size() < 5) {
                outError = "malformed point: " + line;
                return nullptr;
            }
            std::uint64_t srcId = std::stoull(tokens[1]);
            double x = std::stod(tokens[2]);
            double y = std::stod(tokens[3]);
            bool fixed = tokens[4] == "1";
            std::uint64_t newId = currentSketch->addPoint(x, y, fixed).id();
            pointMap[srcId] = newId;
        } else if (tokens[0] == "line") {
            if (tokens.size() < 4) {
                outError = "malformed line: " + line;
                return nullptr;
            }
            std::uint64_t srcId = std::stoull(tokens[1]);
            std::uint64_t srcStart = std::stoull(tokens[2]);
            std::uint64_t srcEnd = std::stoull(tokens[3]);
            auto itS = pointMap.find(srcStart);
            auto itE = pointMap.find(srcEnd);
            if (itS == pointMap.end() || itE == pointMap.end()) {
                outError = "line references unknown point";
                return nullptr;
            }
            std::uint64_t newId = currentSketch->addLine(
                itS->second, itE->second).id();
            lineMap[srcId] = newId;
            pointMap[srcId] = newId;  // line ids also tracked for constraint refs
        } else if (tokens[0] == "constraint") {
            // "    constraint <newId> <type> <value> [p<srcId> p<srcId> ...] [l<srcId> ...]"
            if (tokens.size() < 4) {
                outError = "malformed constraint: " + line;
                return nullptr;
            }
            PendingConstraint pc;
            pc.newId = std::stoull(tokens[1]);
            pc.type = stringToConstraintType(tokens[2]);
            pc.value = std::stod(tokens[3]);
            for (size_t i = 4; i < tokens.size(); ++i) {
                const std::string& t = tokens[i];
                if (t.size() > 1 && t[0] == 'p') {
                    pc.pointSrcIds.push_back(std::stoull(t.substr(1)));
                } else if (t.size() > 1 && t[0] == 'l') {
                    pc.lineSrcIds.push_back(std::stoull(t.substr(1)));
                } else {
                    outError = "bad constraint binding token: " + t;
                    return nullptr;
                }
            }
            pendingConstraints.push_back(std::move(pc));
        } else if (tokens[0] == "param") {
            if (tokens.size() < 3) {
                outError = "malformed param: " + line;
                return nullptr;
            }
            if (currentFeature != nullptr) {
                const double value = std::stod(tokens[2]);
                currentFeature->setParam(tokens[1], value);
            } else {
                outError = "param outside of a feature";
                return nullptr;
            }
        } else if (tokens[0] == "dependency") {
            if (tokens.size() < 2) {
                outError = "malformed dependency: " + line;
                return nullptr;
            }
            std::uint64_t srcDep = std::stoull(tokens[1]);
            auto it = featureMap.find(srcDep);
            if (it == featureMap.end()) {
                outError = "dependency references unknown feature";
                return nullptr;
            }
            currentFeature->addDependency(it->second);
        } else if (tokens[0] == "endpart") {
            state = TopLevel;
            currentPart = nullptr;
            currentFeature = nullptr;
            currentSketch = nullptr;
        } else {
            outError = "unexpected feature-level token: " + tokens[0];
            return nullptr;
        }
    }

    // Resolve deferred constraints: remap source point/line ids to fresh
    // ids assigned by the Sketch's internal generators.
    for (const auto& partPtr : doc->parts()) {
        const Part& part = *partPtr;
        for (const auto& featPtr : part.features()) {
            Feature& feat = *featPtr;
            if (feat.type() != FeatureType::Sketch) continue;
            Sketch* sketch = dynamic_cast<Sketch*>(&feat);
            if (!sketch) continue;
            for (const auto& pc : pendingConstraints) {
                // Determine if this constraint belongs to this sketch by
                // checking whether any of its referenced points/lines are
                // owned by this sketch.
                std::vector<std::uint64_t> mappedPoints;
                std::vector<std::uint64_t> mappedLines;
                bool owns = false;
                for (std::uint64_t src : pc.pointSrcIds) {
                    auto pit = pointMap.find(src);
                    if (pit == pointMap.end()) continue;
                    for (const auto& pt : sketch->points()) {
                        if (pt->id() == pit->second) { owns = true; break; }
                    }
                    mappedPoints.push_back(pit->second);
                }
                for (std::uint64_t src : pc.lineSrcIds) {
                    auto lit = lineMap.find(src);
                    if (lit == lineMap.end()) continue;
                    for (const auto& ln : sketch->lines()) {
                        if (ln->id() == lit->second) { owns = true; break; }
                    }
                    mappedLines.push_back(lit->second);
                }
                if (owns) {
                    sketch->addConstraint(pc.type, pc.value,
                                         std::move(mappedPoints),
                                         std::move(mappedLines));
                }
            }
        }
    }

    return doc;
}

// ---- file helpers ----

bool saveToFile(const Document& doc, const std::string& filepath)
{
    std::ofstream ofs(filepath);
    if (!ofs) return false;
    ofs << serialize(doc);
    return ofs.good();
}

std::unique_ptr<Document> loadFromFile(const std::string& filepath,
                                       std::string& outError)
{
    std::ifstream ifs(filepath);
    if (!ifs) {
        outError = "cannot open file: " + filepath;
        return nullptr;
    }
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return deserialize(ss.str(), outError);
}

} // namespace serialization
} // namespace cadalytic
