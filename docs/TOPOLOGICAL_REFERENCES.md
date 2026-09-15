# Topological References

Topological naming is a programme of provenance, matching, persistence, and tests. It is not a single stable face-number class.

## Identity model

Document objects have stable IDs. Generated sub-elements have a feature-scoped identity composed of feature provenance, element kind, geometric signature, semantic role, and adjacency evidence. OCCT enumeration order, memory addresses, and transient handles are not persistent identity.

## Matching

A rebuild first matches exact provenance and signature, then semantic role and adjacency, then approved geometric fallback rules. Each match records its evidence and confidence. Zero matches produce unresolved references; more than one equally valid match produces an ambiguity diagnostic.

## Lifecycle and persistence

References distinguish resolved, unresolved, deleted, and ambiguous. The file schema stores the source feature ID, element kind, requested name/signature, provenance token, and resolution diagnostics. Reload must preserve the request even when it cannot currently resolve.

## Required tests

Add tests for unchanged rebuild, dimension change, inserted and suppressed upstream features, Boolean split and merge, face deletion, edge-count change, ambiguous replacement, and save/reload/rebuild. Property checks must accompany topology checks.
