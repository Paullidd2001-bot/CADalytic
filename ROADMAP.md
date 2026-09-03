# CADalytic Development Roadmap

## Overview

CADalytic's development is organized into **9 phases** with **26 major components** (A–Z). Each phase builds on the previous one, progressing from core foundations to deployment.

---

## PHASE 1: The Core Engine (Foundations)

**Goal:** Build the non-negotiable systems. Without these, nothing else works.

### Component H: Geometry Kernel Wrapper
- **Issue:** #1
- **Description:** Clean OCCT façade for basic shapes, Boolean operations, transforms, and tessellation for viewport.
- **Dependencies:** None (external: OCCT)
- **Key Files:**
  - `geometry/kernel/OcctKernel.h`
  - `geometry/kernel/OcctKernel.cpp`
  - `geometry/kernel/Facade.h`

### Component C: Data Model
- **Issue:** #2
- **Description:** Hierarchical parametric structure: Document → Part → Feature → Sketch → Constraint
- **Dependencies:** None (internal)
- **Key Files:**
  - `core/document/Document.h`
  - `core/feature/Feature.h`
  - `core/sketch/Sketch.h`
  - `core/constraint/Constraint.h`

### Component D: Rebuild Algorithm
- **Issue:** #3
- **Description:** Dependency graph, dirty flag propagation, partial rebuild, caching, failure recovery.
- **Dependencies:** Component C (Data Model)
- **Key Files:**
  - `rebuild/RebuildEngine.h`
  - `rebuild/RebuildEngine.cpp`
  - `core/dependency/DependencyGraph.h`

### Component F: Topological Naming System
- **Issue:** #4
- **Description:** Stable face/edge/vertex identification across rebuilds using signatures, adjacency, semantic tagging.
- **Dependencies:** Component H (Geometry Kernel)
- **Key Files:**
  - `rebuild/TopologicalNaming.h`
  - `rebuild/TopologicalNaming.cpp`

### Component L: Rebuild-Safe Reference System
- **Issue:** #5
- **Description:** Stable references to geometry and features, integrated with Component F.
- **Dependencies:** Components D, F
- **Key Files:**
  - `rebuild/RebuildSafeReferences.h`
  - `rebuild/RebuildSafeReferences.cpp`

---

## PHASE 2: Sketching & Solving (User-visible core)

**Goal:** Make CADalytic usable for parametric sketching.

### Component O: Sketch Constraint Solver
- **Issue:** #6
- **Description:** 2D solver, DOF analysis, constraint graph, numeric engine.
- **Dependencies:** Components C, D
- **Key Files:**
  - `solver/sketch/SketchSolver.h`
  - `solver/sketch/DOFAnalyzer.h`

### Component I: Command System
- **Issue:** #7
- **Description:** Declarative commands, undo/redo, UI → core mapping.
- **Dependencies:** Component C (Data Model)
- **Key Files:**
  - `command/Command.h`
  - `command/CommandManager.h`
  - `command/CommandManager.cpp`

### Component J: Event System
- **Issue:** #8
- **Description:** Signals/slots for document changes, selection, rebuild events, UI updates.
- **Dependencies:** None (internal)
- **Key Files:**
  - `events/EventBus.h`
  - `events/EventBus.cpp`
  - `events/EventTypes.h`

---

## PHASE 3: 3D Features & Modeling

**Goal:** Enable real part modeling.

### Component R: Constraint-Driven 3D Feature Solver
- **Issue:** #9
- **Description:** Extrude, revolve, fillet, chamfer, shell, loft — all parametric and rebuild-aware.
- **Dependencies:** Components O, D, H
- **Key Files:**
  - `solver/features/FeatureSolver.h`
  - `solver/features/FeatureSolver.cpp`

### Component K: UI Architecture
- **Issue:** #10
- **Description:** Qt dockers, viewport, property inspector, command panels, modular workbenches.
- **Dependencies:** Components I, J, C
- **Key Files:**
  - `ui/MainWindow.h`
  - `ui/widgets/Viewport.h`
  - `ui/dockers/TreeDocker.h`

### Component M: File Format
- **Issue:** #11
- **Description:** Native CADalytic format with versioning, compression, embedded scripts.
- **Dependencies:** Component C (Data Model)
- **Key Files:**
  - `serialization/FileFormat.h`
  - `serialization/Serializer.h`
  - `serialization/Deserializer.h`

---

## PHASE 4: Scripting, Plugins, and Extensibility

**Goal:** Make CADalytic a platform.

### Component E: Python API
- **Issue:** #12
- **Description:** Bindings exposing Document, Features, Geometry, Solver, CAM.
- **Dependencies:** Components C, H, O, R
- **Key Files:**
  - `scripting/python/PythonAPI.h`
  - `scripting/python/Bindings.cpp`
  - `scripting/api/DocumentAPI.h`

### Component G: Plugin System
- **Issue:** #13
- **Description:** Python + C++ plugins, workbenches, custom features, toolpath generators, UI extensions.
- **Dependencies:** Components E, I, K
- **Key Files:**
  - `plugins/PluginManager.h`
  - `plugins/PluginManager.cpp`
  - `plugins/api/PluginInterface.h`

---

## PHASE 5: Assemblies & Advanced Solvers

**Goal:** Support multi-part assemblies.

### Component N: Assembly Constraint Solver
- **Issue:** #14
- **Description:** DOF analysis, kinematic chains, joints, stability.
- **Dependencies:** Components O, D
- **Key Files:**
  - `solver/assembly/AssemblySolver.h`
  - `solver/assembly/AssemblySolver.cpp`

---

## PHASE 6: CAM Engine

**Goal:** Turn CADalytic into a CAD/CAM hybrid.

### Component P: CAM Toolpath Engine
- **Issue:** #15
- **Description:** Roughing, adaptive, finishing, rest machining, stock simulation, post-processing.
- **Dependencies:** Components R, H, S
- **Key Files:**
  - `cam/CAMEngine.h`
  - `cam/toolpath/ToolpathGenerator.h`
  - `cam/simulation/StockSimulator.h`

---

## PHASE 7: Rendering, Simulation, and Analysis

**Goal:** Improve visual quality and enable analysis.

### Component Q: Rendering Engine
- **Issue:** #16
- **Description:** Shaded view, hidden-line, section views, selection buffers.
- **Dependencies:** Components K, H
- **Key Files:**
  - `rendering/Renderer.h`
  - `rendering/Renderer.cpp`

### Component S: Meshing & Analysis
- **Issue:** #17
- **Description:** Tessellation, STL export, FEM pre-processing.
- **Dependencies:** Component H
- **Key Files:**
  - `analysis/MeshGenerator.h`
  - `analysis/MeshGenerator.cpp`

### Component T: Physics / Simulation Hooks
- **Issue:** #18
- **Description:** Optional kinematics, motion, FEM modules.
- **Dependencies:** Components N, S
- **Key Files:**
  - `analysis/SimulationHooks.h`

---

## PHASE 8: Collaboration, Performance, and Tooling

**Goal:** Stabilize and optimize the system.

### Component V: Undo/Redo Transaction System
- **Issue:** #19
- **Description:** Persistent, rebuild-aware, delta compression.
- **Dependencies:** Component I (Command System)
- **Key Files:**
  - `command/TransactionManager.h`

### Component U: Cloud Sync & Collaboration
- **Issue:** #20
- **Description:** Multi-user editing, conflict resolution, version history.
- **Dependencies:** Component M (File Format), Component C (Data Model)
- **Key Files:**
  - `(future) cloud/CloudSyncEngine.h`

### Component Y: Performance & Parallelism
- **Issue:** #21
- **Description:** Thread pools, task graphs, GPU offloading.
- **Dependencies:** Component D (Rebuild Engine)
- **Key Files:**
  - `performance/ThreadPool.h`
  - `performance/TaskGraph.h`

### Component X: Testing Infrastructure
- **Issue:** #22
- **Description:** Unit tests, solver tests, geometry regression, CAM verification.
- **Dependencies:** All components
- **Key Files:**
  - `tests/unit/`
  - `tests/integration/`

### Component Z: Developer Tooling & Diagnostics
- **Issue:** #23
- **Description:** Constraint graph inspector, rebuild profiler, CAM debugger.
- **Dependencies:** All components
- **Key Files:**
  - `tools/diagnostics/ConstraintGraphInspector.h`
  - `tools/diagnostics/RebuildProfiler.h`

---

## PHASE 9: Packaging & Deployment

**Goal:** Prepare for release.

### Component W: Licensing / Packaging / Deployment
- **Issue:** #24
- **Description:** Plugin signing, sandboxing, distribution, updates, installers.
- **Dependencies:** All components
- **Key Files:**
  - `(future) deployment/LicenseManager.h`

---

## Dependency Graph

```
Phase 1:
  H (Geometry Kernel) ────┐
                           └──→ C (Data Model)
                                  ├──→ D (Rebuild Algorithm)
                                  ├──→ L (Rebuild-Safe Refs)
                                  └──→ F (Topological Naming)

Phase 2:
  C, D ──→ O (Sketch Solver)
  C ──→ I (Command System)
  I, J ──→ Phase 3 UI

Phase 3:
  O, D, H ──→ R (3D Features)
  I, J, C ──→ K (UI)
  C ──→ M (File Format)

Phase 4:
  C, H, O, R ──→ E (Python API)
  E, I, K ──→ G (Plugins)

Phase 5:
  O, D ──→ N (Assembly)

Phase 6:
  R, H, S ──→ P (CAM)

Phase 7:
  K, H ──→ Q (Rendering)
  H ──→ S (Meshing)
  N, S ──→ T (Physics)

Phase 8:
  I ──→ V (Transaction System)
  M, C ──→ U (Cloud Sync)
  D ──→ Y (Performance)
  All ──→ X (Testing)
  All ──→ Z (Diagnostics)

Phase 9:
  All ──→ W (Licensing)
```

---

## Recommended Build Order (Shortest Path to Usable CAD)

1. **Phase 1** — Complete: H, C, D, F, L
2. **Phase 2** — Complete: O, I, J
3. **Phase 3** — Partial: R, K, M
4. **Phase 4** — Partial: E, G
5. Then: Phases 5–9 as needed

This sequence gets you a usable parametric CAD tool with sketching, basic 3D features, and a functional UI.

---

## Issue Tracking

All components are tracked as GitHub Issues with:
- Clear descriptions and acceptance criteria
- Dependency relationships (blocking/depends-on)
- Phase assignment
- Estimated effort

See Issues #1–#24 for details.

