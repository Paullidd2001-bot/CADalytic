#pragma once

#include <string>
#include <sstream>

#include "document/Document.h"
#include "document/Part.h"

namespace cadalytic {

namespace serialization {

/// Serialize / deserialize a Document to a compact, versioned text format.
///
/// Format grammar (line-oriented, human-readable):
///   CADALYTIC_DOC v1
///   name <document name>
///   <part-id> <part-name>
///     <feature-type> <feature-id> <feature-name>
///       <field lines, type-specific>
///   ...
///   end
///
/// Feature types: SKETCH, FEATURE
/// Sketch field lines:
///   point <id> <x> <y> <fixed>
///   line <id> <startPointId> <endPointId>
///   constraint <id> <type> <value> [p<pointSrcId> ...] [l<lineSrcId> ...]
/// Feature field lines (any type):
///   param <name> <value>
///   dependency <featureId>
///
/// IDs are document-relative. On load, IDs are remapped to fresh values
/// from the target Document's generators, so round-trip is stable.

/// Serialize the document to a string.
std::string serialize(const Document& doc);

/// Deserialize from a string. Returns the reconstructed Document, or an
/// empty pointer with `outError` populated on failure.
std::unique_ptr<Document> deserialize(const std::string& text,
                                      std::string& outError);

/// Convenience: write to a file. Returns true on success.
bool saveToFile(const Document& doc, const std::string& filepath);

/// Convenience: load from a file. Returns null on failure.
std::unique_ptr<Document> loadFromFile(const std::string& filepath,
                                       std::string& outError);

} // namespace serialization

} // namespace cadalytic
