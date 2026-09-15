# CADalytic Architecture

This document describes the current repository and the target boundaries. A named subsystem is
not evidence that the subsystem exists.

## Current implementation map

| Area | Status | Evidence |
| --- | --- | --- |
| Core document, part, feature and sketch model | implemented but insufficiently tested | `core/` contains concrete classes and tests; identity is process-local |
| Dependency graph and rebuild | partially implemented | `core/dependency/` and `core/rebuild/` propagate dirty state and detect cycles, but lack transaction, failure-state and cancellation policy |
| Sketch solver | implemented but insufficiently tested | `solver/sketch/` handles a small constraint subset; convergence is not a complete constraint-state diagnosis |
| Native serialisation | implemented but insufficiently tested | `serialization/` has text round trips and a `v1` envelope; migrations and unknown-data handling are absent |
| Geometry boundary | scaffold only | `geometry/include/cadalytic/` has facade-shaped types, while `solver/features/` and `src/Occt3DView.*` include OCCT directly |
| OCCT feature operations | partially implemented | `solver/features/FeatureSolver.*` evaluates several primitives and operations directly with `TopoDS_Shape` |
| Qt viewport | scaffold only | `src/Occt3DView.*` renders demo shapes; it is not connected to document features |
| Commands and events | implemented but insufficiently tested | `command/` and `events/` exist; command history is not transaction management |
| Drawings, assemblies, CAM, Python, plugins, analysis | absent | no corresponding tracked implementation directories |

## Target dependency direction

```text
UI -> application commands -> document and transactions -> dependency/rebuild
  -> application geometry interfaces -> OCCT adapter
```

Core and document code must not depend on Qt Widgets. Document, rebuild, and feature contracts must
not expose OCCT types. UI may consume read-only presentation data and invoke commands, but must not
mutate model containers directly. Tests may depend on production modules; production code must not
depend on tests.

## Contracts to establish

These are architectural contracts, not claims about current compliance:

- Object identity is stable within a document and survives save/load through an explicit schema.
- Model mutation occurs inside a document transaction; command history is a consumer of transactions.
- Rebuild uses a directed graph with explicit dependency direction, stable scheduling, cycle errors,
  feature states, and a defined last-valid-result or rollback policy.
- Application geometry owns shape handles and diagnostics. OCCT remains in an adapter and approved
  presentation integration points until migrated.
- References use feature provenance and application-level topological identities; transient OCCT
  indices and pointer identity are never persisted.
- Quantities carry units or dimension types. Internal length and mass conventions are explicit.
- Errors are returned as structured diagnostics at subsystem boundaries; failures do not silently
  invalidate a document.

## Delivery rule

The first meaningful product milestone is a headless, tested document-to-sketch-to-extrusion flow
that saves, reloads, changes a dimension, rebuilds, validates the solid, and reports reference
failure explicitly. See [ROADMAP.md](ROADMAP.md).
