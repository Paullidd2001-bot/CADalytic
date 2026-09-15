# Current State Assessment

Assessment based on the tracked tree at commit `bb91960` and the available local toolchain. The status labels are deliberately conservative.

| Subsystem | Current files | Status | Evidence | Architectural concern | Dependency concern | Tests present | Immediate next action | Priority |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Document and part model | `core/document/*`, `core/feature/*` | implemented but insufficiently tested | Concrete ownership and add/remove operations exist | IDs are process-local counters; no transaction or lifecycle contract | Core is independent of Qt and OCCT | `core_model_tests`, serialisation fixture | Define stable identity and transaction boundary | 1 |
| Sketch data | `core/sketch/*`, `core/constraint/*` | implemented but insufficiently tested | Points, lines and constraints are stored | Geometry and constraints are not yet a complete parametric profile model | Core remains headless | solver and core tests | Specify profile validity and constraint state model | 2 |
| Dependency graph | `core/dependency/*` | partially implemented | Edges, transitive dirty marking and cycle detection exist | Unordered-map traversal can make rebuild order unstable; errors are only strings | No external dependency | rebuild tests | Add deterministic ordering and graph diagnostics | 1 |
| Rebuild engine | `core/rebuild/RebuildEngine.*` | partially implemented | Builds a graph and calls `Feature::rebuild()` | No feature states, rollback, cancellation, downstream failure policy or transaction | No Qt/OCCT dependency | rebuild tests | Define state machine and failure policy | 1 |
| Topological naming | `core/rebuild/TopologicalNaming.*` | partially implemented | Signature, adjacency and semantic-tag matching code exists | No proven OCCT fact extraction or persistence; ambiguity policy is incomplete | Intended boundary is good but not enforced by build | rebuild/reference tests | Add ambiguity and save/reload tests | 2 |
| Rebuild-safe references | `core/rebuild/RebuildSafeReferences.*` | partially implemented | Declarative feature/element/signature references exist | References are not part of the document schema and cannot prove survival across regeneration | Depends on naming implementation | rebuild/reference tests | Define provenance and persistence format | 2 |
| Geometry interface | `geometry/include/cadalytic/*` | scaffold only | Shape/result/operation headers exist; several sources are empty | Interface is too incomplete to own the vertical slice | Feature solver bypasses it | limited geometry coverage | Choose minimum application geometry contract | 1 |
| OCCT feature solver | `solver/features/*` | partially implemented | Primitives, profile operations and booleans are evaluated using OCCT | `TopoDS_Shape` is public in the solver API; validity/mass checks are weak | Direct OCCT coupling blocks clean core boundary | feature solver tests | Move result ownership behind adapter incrementally | 2 |
| Sketch solver | `solver/sketch/*` | implemented but insufficiently tested | Several point constraints solve test fixtures | Result reports success but not complete constraint diagnosis | Headless and separable | solver tests | Add under/over/conflicting cases and DOF evidence | 2 |
| Serialisation | `serialization/*` | implemented but insufficiently tested | Text round trip, compression and `v1` envelope exist | No migration registry, schema payload version, unknown-field policy or shape persistence | Headless | serialisation tests | Add versioned schema and identity round-trip tests | 1 |
| Commands/events | `command/*`, `events/*` | implemented but insufficiently tested | Command classes and event bus compile in focused tests | Commands are not transactions; event re-entrancy is unspecified | Headless modules are separated | command/event tests | Specify mutation and event ownership rules | 2 |
| Viewport/UI | `src/*` | scaffold only | Qt/OCCT window renders demonstration solids | Not connected to document state; OCCT and Qt are presentation concerns only | Application target requires Qt and OCCT | no GUI test | Connect read-only presentation after Gate 4 | 3 |
| CMake integration | root and module `CMakeLists.txt` | partially implemented | Targets and CTest registrations exist | Root requires unavailable Qt/OCCT; newer geometry sources are not clearly included; no presets | Platform configuration is implicit | CTest declarations | Create headless target and presets | 1 |
| CI | `.github/workflows/core-tests.yml` | scaffold only | Ubuntu workflow contains direct compiler commands | Workflow is incomplete/malformed and omits Windows; it does not represent CMake | Linux-only | intended test commands | Replace with minimal CMake matrix | 1 |
| Drawings | absent | absent | No tracked drawing module | No model association contract | None | none | Reserve architecture only | 4 |
| Materials and units | absent | absent | No typed quantity/material module | Numeric parameters are untyped doubles | None | none | Define units before physical properties | 3 |
| Assemblies, CAM, scripting, plugins, analysis | absent | absent | No tracked implementation directories | Must wait for stable model and references | None | none | Keep deferred behind roadmap gates | 5 |

## Baseline limitations

CMake was not available on the assessment shell, so a fresh configure/build was not possible. The
repository's local PowerShell runner was blocked by execution policy. Existing `.exe` artefacts were
present but Windows returned access denied when launched; they are not accepted as fresh test evidence.
No build or test pass is claimed from this environment.
