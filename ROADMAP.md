# CADalytic Development Roadmap

This roadmap is ordered by architectural risk and executable evidence, not by the number of
directories or feature names. Planned work must not be described as implemented work.

## Gates

### Gate 0: Reproducible build and tests

Acceptance: configure and build the headless targets with CMake, run every registered test with
`ctest`, and establish Windows and Linux CI jobs. Record compiler, Qt, and OCCT versions.

### Gate 1: Document and identity foundation

Define ownership, stable object IDs, lifecycle, typed properties, schema versioning, transactions,
and separation of model state from generated/cache state. Add unit tests for save/load identity and
transaction boundaries.

### Gate 2: Deterministic dependency and rebuild engine

Implement explicit graph semantics, transitive dirty propagation, stable scheduling, cycle errors,
failed-feature and downstream states, cancellation, diagnostics, and last-valid-result or rollback
behaviour. Test each policy headlessly.

### Gate 3: Geometry adapter and validity framework

Complete the application geometry interface for the required first vertical slice. Contain OCCT in
the adapter, return structured diagnostics, and validate solids and relevant mass properties.

### Gate 4: Reference-safe parametric vertical slice

Create a document, constrained sketch, extrusion, save, reload, dimension change, rebuild, validity
check, and explicit reference survival/failure result in one automated test. Add undo/redo only after
transactions are independently defined.

### Gate 5: Usable constrained sketcher

Separate sketch data, solver adapter, result and diagnosis. Report solved, under-constrained,
fully-constrained, over-constrained, conflicting, and numerically failed states with tests.

### Gate 6: Core part-modelling features

Add feature contracts and tests for datum plane/axis, sketch, extrusion, revolution, hole, fillet,
and chamfer. Each feature needs validation, failure diagnostics, property checks, and reference
behaviour.

### Gate 7: Engineering viewport

Connect document results to a basic shaded/wireframe OCCT presentation with selection, standard
views, fit-to-view, sketch/constraint display, and failed/suppressed feature diagnostics.

### Gate 8: Drawing minimum viable product

Implement sheet/view data, base and projected views, hidden lines, sections, basic dimensions,
title block, PDF output, and broken model-association diagnostics.

### Gate 9: Materials and physical properties

Define units, material assignment, density, mass, volume, area, centre of mass, and tolerances with
headless property tests.

### Gate 10: Assembly minimum viable product

Add component instances, document references, fixed placement, transform persistence, missing
reference diagnostics, then introduce constraints only after part references are stable.

### Gate 11: Automation and extension API

Expose documented application-level Python/plugin APIs that require transactions, validation, and
tracked dependencies. Do not expose internal containers or unsafe kernel pointers.

### Gate 12: Specialist analysis and CAM work

Add clean export and analysis hand-off first. Consider CAM only after modelling, references,
drawings, materials, and validation are stable. AI-assisted checks remain advisory and outside
deterministic core modelling.

## Continuous requirements

Testing, reference models, cross-platform CI, architecture decision records, dependency checks,
diagnostics, and documentation advance with every gate. No gate is complete because code compiles;
its acceptance evidence must be executable where practical.

## Scope exclusions for the current foundation task

Do not implement a complete sketch solver, topological naming solution, drawing package, assembly
solver, CAM engine, plugin marketplace, or AI subsystem as part of this documentation and baseline
work.

## Highest risks

The immediate risks are OCCT leakage, process-local identity, incomplete rebuild failure semantics,
non-deterministic graph iteration, weak reference persistence, and a broad planned architecture that
does not yet have a tested vertical slice.
