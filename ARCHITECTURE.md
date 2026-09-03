# CADalytic Architecture Overview

## High-Level System Map

```
┌─────────────────────────────────────────────────────────────────┐
│                          UI LAYER (K)                            │
│  Qt6 Widgets | Dockers | Viewport | Property Inspector | Panels  │
└────────────────────────────┬────────────────────────────────────┘
                             │
┌────────────────────────────▼────────────────────────────────────┐
│                     COMMAND SYSTEM (I)                           │
│         Declarative Commands | Undo/Redo | UI → Core Mapping   │
└────────────────────────────┬────────────────────────────────────┘
                             │
┌────────────────────────────▼────────────────────────────────────┐
│                   EVENT SYSTEM (J)                               │
│  Signals/Slots | Rebuild Events | Selection | Document Changes │
└────────────────────────────┬────────────────────────────────────┘
                             │
┌────────────────────────────▼────────────────────────────────────┐
│                     DOCUMENT MODEL (C)                           │
│  Document → Part → Feature → Sketch → Constraint               │
│         Hierarchical Parametric Structure                        │
└────────────────────────────┬────────────────────────────────────┘
                             │
┌────────────────────────────▼────────────────────────────────────┐
│                    REBUILD ENGINE (D)                            │
│  Dependency Graph | Dirty Propagation | Partial Rebuilds        │
└────────────────────────────┬────────────────────────────────────┘
                             │
        ┌────────────────────┼────────────────────┐
        │                    │                    │
        ▼                    ▼                    ▼
┌──────────────────┐ ┌──────────────────┐ ┌──────────────────┐
│  CONSTRAINT      │ │  GEOMETRY        │ │  SERIALIZATION   │
│  SOLVERS (O,N,R) │ │  KERNEL (H)      │ │  (M)             │
│                  │ │  OCCT Facade     │ │  File Format     │
│ Sketch (O)       │ │  Tessellation    │ │  Native Format   │
│ 3D Features (R)  │ │  Shapes/Boolean  │ │  Compression     │
│ Assembly (N)     │ │  Transforms      │ │  Versioning      │
└──────────────────┘ └──────────────────┘ └──────────────────┘
        │                    │
        │  Topological Naming (F) & Rebuild-Safe References (L)
        │
┌──────────────────┐ ┌──────────────────┐ ┌──────────────────┐
│  RENDERING (Q)   │ │  ANALYSIS (S,T)  │ │  CAM ENGINE (P)  │
│  Shaded/Hidden   │ │  Meshing         │ │  Toolpaths       │
│  Sections        │ │  FEM Pre-proc    │ │  Roughing/Fin.   │
│  Selection Buf.  │ │  Simulation      │ │  Verification    │
└──────────────────┘ └──────────────────┘ └──────────────────┘
        │                    │                    │
        └────────────────────┼────────────────────┘
                             │
┌────────────────────────────▼────────────────────────────────────┐
│              EXTENSIBILITY & PERFORMANCE                         │
│                                                                   │
│  Python API (E) | Plugin System (G) | Thread Pools (Y)          │
│  Cloud Sync (U) | Transaction System (V) | Parallelism (Y)      │
│  Developer Tools (Z) | Testing (X) | Licensing (W)              │
└─────────────────────────────────────────────────────────────────┘
```

## Folder Structure

```
CADalytic/
├── src/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── MainWindow.cpp
│   ├── MainWindow.h
│   └── app/
│       └── Application.h
│
├── core/
│   ├── CMakeLists.txt
│   ├── document/
│   │   ├── Document.h
│   │   ├── Document.cpp
│   │   ├── Part.h
│   │   ├── Part.cpp
│   │   └── CMakeLists.txt
│   ├── feature/
│   │   ├── Feature.h
│   │   ├── Feature.cpp
│   │   ├── FeatureType.h
│   │   └── CMakeLists.txt
│   ├── sketch/
│   │   ├── Sketch.h
│   │   ├── Sketch.cpp
│   │   └── CMakeLists.txt
│   ├── constraint/
│   │   ├── Constraint.h
│   │   ├── Constraint.cpp
│   │   ├── ConstraintType.h
│   │   └── CMakeLists.txt
│   └── dependency/
│       ├── DependencyGraph.h
│       ├── DependencyGraph.cpp
│       └── CMakeLists.txt
│
├── geometry/
│   ├── CMakeLists.txt
│   ├── kernel/
│   │   ├── OcctKernel.h
│   │   ├── OcctKernel.cpp
│   │   ├── Facade.h
│   │   └── CMakeLists.txt
│   ├── Shape.h
│   ├── Shape.cpp
│   ├── Box.cpp
│   ├── Cylinder.cpp
│   ├── Line3D.h
│   ├── Line3D.cpp
│   ├── Plane3D.h
│   ├── Plane3D.cpp
│   ├── Ray3D.h
│   └── Ray3D.cpp
│
├── solver/
│   ├── CMakeLists.txt
│   ├── sketch/
│   │   ├── SketchSolver.h
│   │   ├── SketchSolver.cpp
│   │   ├── DOFAnalyzer.h
│   │   └── CMakeLists.txt
│   ├── features/
│   │   ├── FeatureSolver.h
│   │   ├── FeatureSolver.cpp
│   │   └── CMakeLists.txt
│   └── assembly/
│       ├── AssemblySolver.h
│       ├── AssemblySolver.cpp
│       └── CMakeLists.txt
│
├── rebuild/
│   ├── CMakeLists.txt
│   ├── RebuildEngine.h
│   ├── RebuildEngine.cpp
│   ├── TopologicalNaming.h
│   ├── TopologicalNaming.cpp
│   ├── RebuildSafeReferences.h
│   └── RebuildSafeReferences.cpp
│
├── ui/
│   ├── CMakeLists.txt
│   ├── MainWindow.h
│   ├── MainWindow.cpp
│   ├── widgets/
│   │   ├── Viewport.h
│   │   ├── Viewport.cpp
│   │   ├── PropertyInspector.h
│   │   ├── PropertyInspector.cpp
│   │   └── CMakeLists.txt
│   ├── dockers/
│   │   ├── TreeDocker.h
│   │   ├── TreeDocker.cpp
│   │   └── CMakeLists.txt
│   └── commands/
│       ├── CommandPanel.h
│       ├── CommandPanel.cpp
│       └── CMakeLists.txt
│
├── command/
│   ├── CMakeLists.txt
│   ├── Command.h
│   ├── Command.cpp
│   ├── CommandManager.h
│   ├── CommandManager.cpp
│   └── commands/
│       └── CMakeLists.txt
│
├── events/
│   ├── CMakeLists.txt
│   ├── EventBus.h
│   ├── EventBus.cpp
│   ├── Event.h
│   └── EventTypes.h
│
├── serialization/
│   ├── CMakeLists.txt
│   ├── FileFormat.h
│   ├── FileFormat.cpp
│   ├── Serializer.h
│   ├── Deserializer.h
│   └── compression/
│       └── CMakeLists.txt
│
├── cam/
│   ├── CMakeLists.txt
│   ├── CAMEngine.h
│   ├── CAMEngine.cpp
│   ├── toolpath/
│   │   ├── ToolpathGenerator.h
│   │   └── CMakeLists.txt
│   └── simulation/
│       ├── StockSimulator.h
│       └── CMakeLists.txt
│
├── scripting/
│   ├── CMakeLists.txt
│   ├── python/
│   │   ├── PythonAPI.h
│   │   ├── Bindings.cpp
│   │   └── CMakeLists.txt
│   └── api/
│       ├── DocumentAPI.h
│       ├── FeatureAPI.h
│       └── CMakeLists.txt
│
├── plugins/
│   ├── CMakeLists.txt
│   ├── PluginManager.h
│   ├── PluginManager.cpp
│   └── api/
│       ├── PluginInterface.h
│       └── CMakeLists.txt
│
├── rendering/
│   ├── CMakeLists.txt
│   ├── Renderer.h
│   ├── Renderer.cpp
│   └── viewers/
│       └── CMakeLists.txt
│
├── analysis/
│   ├── CMakeLists.txt
│   ├── MeshGenerator.h
│   ├── MeshGenerator.cpp
│   └── CMakeLists.txt
│
├── performance/
│   ├── CMakeLists.txt
│   ├── ThreadPool.h
│   ├── ThreadPool.cpp
│   ├── TaskGraph.h
│   └── CMakeLists.txt
│
├── tests/
│   ├── CMakeLists.txt
│   ├── unit/
│   │   ├── CMakeLists.txt
│   │   ├── test_document.cpp
│   │   ├── test_solver.cpp
│   │   └── test_geometry.cpp
│   └── integration/
│       ├── CMakeLists.txt
│       └── test_rebuild.cpp
│
├── tools/
│   ├── CMakeLists.txt
│   └── diagnostics/
│       ├── ConstraintGraphInspector.h
│       ├── RebuildProfiler.h
│       └── CMakeLists.txt
│
├── CMakeLists.txt
├── ARCHITECTURE.md
├── ROADMAP.md
└── README.md
```

## Key Design Principles

1. **Modularity** — Each subsystem (geometry, solver, UI, etc.) is independently compilable.
2. **Clean Abstractions** — OCCT is wrapped; internal code doesn't leak OCCT types.
3. **Dependency Inversion** — Command and Event systems decouple UI from core logic.
4. **Incremental Rebuild** — The DependencyGraph + RebuildEngine enable partial updates.
5. **Extensibility** — Python API and Plugin System allow third-party extensions.
6. **Testability** — Core logic is isolated from UI; solvers and geometry have unit tests.

## Build Flow

```
CMakeLists.txt (root)
├── src/CMakeLists.txt → executable (cadalytic)
├── core/CMakeLists.txt → core library
│   ├── document/CMakeLists.txt
│   ├── feature/CMakeLists.txt
│   ├── sketch/CMakeLists.txt
│   ├── constraint/CMakeLists.txt
│   └── dependency/CMakeLists.txt
├── geometry/CMakeLists.txt → geometry library
├── solver/CMakeLists.txt → solver library
├── rebuild/CMakeLists.txt → rebuild library
├── ui/CMakeLists.txt → ui library
├── command/CMakeLists.txt → command library
├── events/CMakeLists.txt → events library
├── serialization/CMakeLists.txt → serialization library
├── cam/CMakeLists.txt → cam library
├── scripting/CMakeLists.txt → scripting library
├── plugins/CMakeLists.txt → plugins library
├── rendering/CMakeLists.txt → rendering library
├── analysis/CMakeLists.txt → analysis library
├── performance/CMakeLists.txt → performance library
├── tests/CMakeLists.txt → test executables
└── tools/CMakeLists.txt → diagnostic tools
```

## Component Reference (A–Z)

| Letter | Component | Path | Status |
|--------|-----------|------|--------|
| A | Full Architecture Diagram | ARCHITECTURE.md | ✓ This file |
| B | C++ Folder Structure | See above | ✓ Planned |
| C | Data Model | core/document/, core/feature/, core/sketch/, core/constraint/ | Phase 1 |
| D | Rebuild Algorithm | rebuild/ | Phase 1 |
| E | Python API | scripting/python/ | Phase 4 |
| F | Topological Naming System | rebuild/TopologicalNaming.h | Phase 1 |
| G | Plugin System | plugins/ | Phase 4 |
| H | Geometry Kernel Wrapper | geometry/kernel/ | Phase 1 |
| I | Command System | command/ | Phase 2 |
| J | Event System | events/ | Phase 2 |
| K | UI Architecture | ui/ | Phase 3 |
| L | Rebuild-Safe Reference System | rebuild/RebuildSafeReferences.h | Phase 1 |
| M | File Format | serialization/ | Phase 3 |
| N | Assembly Constraint Solver | solver/assembly/ | Phase 5 |
| O | Sketch Constraint Solver | solver/sketch/ | Phase 2 |
| P | CAM Toolpath Engine | cam/ | Phase 6 |
| Q | Rendering Engine | rendering/ | Phase 7 |
| R | 3D Feature Solver | solver/features/ | Phase 3 |
| S | Meshing & Analysis | analysis/ | Phase 7 |
| T | Physics / Simulation Hooks | analysis/ | Phase 7 |
| U | Cloud Sync & Collaboration | (future) | Phase 8 |
| V | Undo/Redo Transaction System | command/CommandManager.h | Phase 8 |
| W | Licensing / Packaging / Deployment | (future) | Phase 9 |
| X | Testing Infrastructure | tests/ | Phase 8 |
| Y | Performance & Parallelism | performance/ | Phase 8 |
| Z | Developer Tooling & Diagnostics | tools/ | Phase 8 |

