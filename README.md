# CADalytic

CADalytic is an experimental cross-platform parametric mechanical CAD application in C++.
The current repository contains a headless document model, dependency/rebuild code, a small
sketch solver, a native text file format, an OCCT-based feature solver, and an OCCT/Qt demo
viewport. It is not yet a complete CAD application.

## Current status

The core model, command/event code, serialisation code, sketch solver, and several OCCT feature
operations have implementation and test sources. The implementation is still an early foundation:
transactions, persistent identity, deterministic rebuild diagnostics, reference persistence, and a
complete application-level geometry boundary are not established. The viewport currently contains
demonstration geometry and is not the modelling workflow.

See [docs/CURRENT_STATE_ASSESSMENT.md](docs/CURRENT_STATE_ASSESSMENT.md) for evidence and known
limitations. The intended boundaries are in [ARCHITECTURE.md](ARCHITECTURE.md), and the gated
development sequence is in [ROADMAP.md](ROADMAP.md).

## Platforms and dependencies

Windows is the currently exercised development platform. Linux is a target platform, but this
checkout does not yet provide equivalent CI or a verified Linux application build.

The application build requires CMake 3.16 or newer, a C++20 compiler, Qt 6 Widgets/OpenGL/
OpenGLWidgets, and Open CASCADE Technology (OCCT). The headless core tests require only a C++20
compiler and the repository sources.

## Build and test

From a configured environment:

```text
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The root CMake project currently requires Qt and OCCT during configuration. See
[docs/CROSS_PLATFORM.md](docs/CROSS_PLATFORM.md) for current toolchain limitations.

## Limitations

- No complete parametric sketch-to-extrusion-save-reload-change-rebuild vertical slice is tested.
- Feature shapes currently expose OCCT types from the feature solver.
- Topological naming and rebuild-safe references are early algorithms, not a persistence guarantee.
- Command history is not a transaction/undo implementation.
- The file format has an envelope version, but no migration framework or unknown-data policy.
- Drawings, assemblies, CAM, Python extensions, plugins, analysis, and AI-assisted checks are planned.

## Project documents

- [Architecture](ARCHITECTURE.md)
- [Roadmap](ROADMAP.md)
- [Current state assessment](docs/CURRENT_STATE_ASSESSMENT.md)
- [Agent development rules](docs/AGENT_DEVELOPMENT.md)
