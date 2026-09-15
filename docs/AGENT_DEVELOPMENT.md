# Agent Development Rules

Agents must inspect the current tree, establish a build/test baseline where possible, and make the smallest reviewable change. They must distinguish implemented, partial, scaffolded, planned, and absent work in documentation.

## Roles

- Core and Document: `core/`; read-only `serialization/`, tests; tests for identity, ownership, properties, and transactions.
- Geometry: `geometry/`; read-only feature solver and OCCT configuration; adapter and validity tests.
- Rebuild and Reference: `core/dependency/`, `core/rebuild/`; graph, failure, naming, and persistence tests.
- Sketch Solver: `solver/sketch/`; read-only sketch model; solver-state and numerical tests.
- Feature: `solver/features/`; read-only geometry and core; feature contracts and property tests.
- UI and Viewport: `src/`; read-only commands and presentation contracts; GUI smoke checks.
- Drawing: future `drawing/`; read-only model/reference APIs; association and export tests.
- Serialisation: `serialization/`; schema, migration, corruption, and round-trip tests.
- Test and Validation: `tests/`, CI; may inspect all production modules but should not alter production behaviour casually.
- Documentation: root docs and `docs/`; must cite repository evidence.

Agents may not change public interfaces without updating consumers and tests, add dependencies without an ADR, bypass the geometry facade or transactions, weaken tests, hide CI failures, or combine several major subsystem changes without explicit review. Equations must state assumptions, units, and validation cases. Escalate uncertain ownership, schema changes, cross-platform failures, and API changes before proceeding.
