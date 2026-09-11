#pragma once

#include <memory>
#include <string>

#include "document/Document.h"

namespace cadalytic {
namespace serialization {

// Component M: the native CADalytic file format.
//
// The on-disk shape is an envelope around the versioned document text:
//
//   CADALYTIC_FILE v1            plain body (DocumentSerializer output)
//   CADALYTIC_FILE v1 Z          RLE-compressed body
//   <body>
//
// The envelope carries:
//   - a magic token so unrelated files are rejected early
//   - a format version for forward migrations
//   - an optional compression marker ('Z') when the body was packed
//
// save()/load() prefer compression and transparently fall back to the plain
// representation when packing would not help.

constexpr int kFormatVersion = 1;
constexpr const char* kFileMagic = "CADALYTIC_FILE";
constexpr const char* kCompressedMarker = "Z";

/// Build the full file text (envelope + body).
std::string toFileText(const Document& doc, bool compress);

/// Parse file text produced by toFileText(). Returns null + outError on a
/// bad envelope, an unknown version, or a corrupt compressed body.
std::unique_ptr<Document> fromFileText(const std::string& text,
                                       std::string& outError);

/// Convenience file helpers. save() defaults to compression.
bool save(const Document& doc, const std::string& filepath);
bool save(const Document& doc, const std::string& filepath, bool compress);
std::unique_ptr<Document> load(const std::string& filepath,
                               std::string& outError);

} // namespace serialization
} // namespace cadalytic