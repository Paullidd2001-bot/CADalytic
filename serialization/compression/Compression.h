#pragma once

#include <string>

namespace cadalytic {
namespace serialization {
namespace compression {

// Dependency-free byte-stream compression used by the native file format.
//
// The scheme is a simple run-length encoding over raw bytes (safe for UTF-8,
// since every byte position round-trips independently):
//
//   [0x00] <len:1> <len literal bytes>   -- literal chunk  (1..255 bytes)
//   [0xFF] <len:1> <byte>                -- run of len copies of byte
//
// Runs shorter than 3 bytes are stored literally. Literal chunks are capped at
// 255 bytes so the stream can always be decoded in one pass. Real CADalytic
// documents are mostly repeated ASCII tokens, so this typically shrinks them;
// when it would not, callers fall back to the uncompressed representation.
//
// All functions operate on raw string bytes and never interpret text.

/// Compress `data`. Returns the compressed byte string.
/// An empty input yields an empty output.
std::string compress(const std::string& data);

/// Decompress a stream produced by compress(). Returns false on a malformed
/// stream; on failure `outError` is populated.
bool decompress(const std::string& data, std::string& out,
                std::string& outError);

} // namespace compression
} // namespace serialization
} // namespace cadalytic