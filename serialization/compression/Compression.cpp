#include "Compression.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace cadalytic {
namespace serialization {
namespace compression {

namespace {

constexpr unsigned char kLiteralMarker = 0x00;
constexpr unsigned char kRepeatMarker = 0xFF;

void emitToken(std::string& out, unsigned char marker)
{
    out.push_back(static_cast<char>(marker));
}

} // namespace

std::string compress(const std::string& data)
{
    std::string output;
    output.reserve(data.size());

    const std::size_t size = data.size();
    std::size_t i = 0;

    std::size_t literalStart = 0;

    auto flushLiteral = [&](std::size_t end) {
        while (literalStart < end) {
            const std::size_t chunk = std::min(end - literalStart, static_cast<std::size_t>(255));
            emitToken(output, kLiteralMarker);
            output.push_back(static_cast<char>(chunk));
            output.append(data, literalStart, chunk);
            literalStart += chunk;
        }
    };

    while (i < size) {
        // Measure the run of identical bytes starting at i.
        std::size_t run = 1;
        while (i + run < size && data[i + run] == data[i] && run < 255) {
            ++run;
        }

        if (run >= 3) {
            flushLiteral(i);
            emitToken(output, kRepeatMarker);
            output.push_back(static_cast<char>(run));
            output.push_back(data[i]);
            i += run;
            literalStart = i;
        } else {
            ++i;
        }
    }
    flushLiteral(size);
    return output;
}

bool decompress(const std::string& data, std::string& out,
                std::string& outError)
{
    out.clear();
    out.reserve(data.size() * 2);

    std::size_t i = 0;
    const std::size_t size = data.size();
    while (i < size) {
        const auto marker = static_cast<unsigned char>(data[i++]);
        if (i >= size) {
            outError = "truncated compression token";
            return false;
        }
        const auto length = static_cast<unsigned char>(data[i++]);
        if (length == 0) {
            outError = "zero-length compression chunk";
            return false;
        }
        if (marker == kRepeatMarker) {
            if (i >= size) {
                outError = "truncated run byte";
                return false;
            }
            out.append(static_cast<std::size_t>(length), data[i++]);
        } else if (marker == kLiteralMarker) {
            if (i + length > size) {
                outError = "literal chunk overruns the stream";
                return false;
            }
            out.append(data, i, length);
            i += length;
        } else {
            outError = "unknown compression marker";
            return false;
        }
    }
    return true;
}

} // namespace compression
} // namespace serialization
} // namespace cadalytic