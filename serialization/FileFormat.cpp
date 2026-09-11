#include "FileFormat.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "compression/Compression.h"
#include "DocumentSerializer.h"

namespace cadalytic {
namespace serialization {

namespace {

std::vector<std::string> splitTokens(const std::string& line)
{
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

} // namespace

std::string toFileText(const Document& doc, bool compress)
{
    std::string envelope;
    std::string body;
    if (compress) {
        const std::string plain = serialize(doc);
        body = compression::compress(plain);
        if (body.size() < plain.size()) {
            envelope = std::string(kFileMagic) + " v" +
                       std::to_string(kFormatVersion) + " " +
                       kCompressedMarker + "\n";
        } else {
            // Compression does not help: store plain and omit the marker.
            envelope = std::string(kFileMagic) + " v" +
                       std::to_string(kFormatVersion) + "\n";
            body = plain;
        }
    } else {
        envelope = std::string(kFileMagic) + " v" +
                   std::to_string(kFormatVersion) + "\n";
        body = serialize(doc);
    }
    return envelope + body;
}

std::unique_ptr<Document> fromFileText(const std::string& text,
                                       std::string& outError)
{
    outError.clear();

    std::istringstream iss(text);
    std::string header;
    if (!std::getline(iss, header)) {
        outError = "empty file";
        return nullptr;
    }

    const auto tokens = splitTokens(header);
    if (tokens.size() < 2 || tokens[0] != kFileMagic) {
        outError = "not a CADalytic file (bad magic)";
        return nullptr;
    }
    if (tokens[1] != "v1") {
        outError = "unsupported CADalytic format version: " + tokens[1];
        return nullptr;
    }

    std::ostringstream bodyStream;
    bodyStream << iss.rdbuf();
    const std::string body = bodyStream.str();
    std::string plain = body;

    const bool compressed = (tokens.size() >= 3 && tokens[2] == kCompressedMarker);
    if (compressed) {
        std::string error;
        if (!compression::decompress(body, plain, error)) {
            outError = "corrupt compressed body: " + error;
            return nullptr;
        }
    }

    return deserialize(plain, outError);
}

bool save(const Document& doc, const std::string& filepath)
{
    return save(doc, filepath, true);
}

bool save(const Document& doc, const std::string& filepath, bool compress)
{
    std::ofstream out(filepath, std::ios::binary);
    if (!out) {
        return false;
    }
    const std::string text = toFileText(doc, compress);
    out.write(text.data(), static_cast<std::streamsize>(text.size()));
    return out.good();
}

std::unique_ptr<Document> load(const std::string& filepath,
                               std::string& outError)
{
    std::ifstream in(filepath, std::ios::binary);
    if (!in) {
        outError = "cannot open file: " + filepath;
        return nullptr;
    }
    std::ostringstream stream;
    stream << in.rdbuf();
    return fromFileText(stream.str(), outError);
}

} // namespace serialization
} // namespace cadalytic