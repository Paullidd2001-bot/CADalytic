# Gate 0 Build Contract

## Scope

Gate 0 establishes a reproducible headless build and test path. It does not alter document behaviour,
geometry ownership, serialisation, transactions, rebuilding, or topological reference algorithms.

## Existing files in scope

- `CMakeLists.txt`: root dependency discovery, targets, and CTest registration.
- `core/CMakeLists.txt`: headless document/model library.
- `serialization/CMakeLists.txt`: headless serialisation library.
- `solver/CMakeLists.txt`: headless sketch solver and currently coupled OCCT feature target.
- `events/CMakeLists.txt` and `command/CMakeLists.txt`: headless support libraries.
- `tests/*.cpp`: existing unit-style executable tests.
- `.github/workflows/core-tests.yml`: current compiler-script workflow.
- `.gitignore`: generated build output policy.

## Contract and public interfaces

The public build interface is the CMake cache option:

```text
CADALYTIC_BUILD_APPLICATION=ON|OFF
```

`ON` retains the Qt and OCCT application/feature build. `OFF` must not call `find_package(Qt6)` or
`find_package(OpenCASCADE)`, must not create the GUI executable or OCCT feature solver target, and
must still provide the headless libraries and tests. Existing C++ public interfaces are unchanged.

## Permitted dependencies

Headless targets may depend on the C++ standard library and other headless CADalytic targets:
`cadalytic_core`, `cadalytic_serialization`, `cadalytic_solver`, `cadalytic_events`, and
`cadalytic_command`. They must not depend on Qt, OCCT, GUI sources, or the OCCT feature solver.

The application path may continue to depend on Qt and OCCT. No new third-party dependency is added.

## Failure behaviour

Configuration fails clearly if `CADALYTIC_BUILD_APPLICATION=ON` is selected without Qt or OCCT.
Configuration with `OFF` remains usable without those packages. Compilation or test failures remain
fatal; CI must not ignore failures or convert them into warnings.

## Acceptance criteria

1. `cmake -S . -B build-headless -DCADALYTIC_BUILD_APPLICATION=OFF` configures.
2. `cmake --build build-headless` builds all headless libraries and registered tests.
3. `ctest --test-dir build-headless --output-on-failure` runs every headless test successfully.
4. Headless configuration does not search for or link Qt or OCCT.
5. The application option remains available for environments with Qt and OCCT.
6. Linux and Windows CI execute the same configure, build, and CTest sequence.
7. No generated files are written to the source tree by the documented commands.

## Required evidence

### Unit tests

Run the existing core model, rebuild, sketch solver, event, command, and serialisation test
executables through CTest. Do not weaken or rewrite them for this milestone.

### Integration tests

Add no new model integration behaviour in Gate 0. The CMake integration itself is tested by
configuring, building, and running the complete registered headless test set.

### Geometric regression tests

None are introduced in Gate 0 because OCCT geometry is explicitly excluded from the headless path.
Existing feature solver geometry tests remain covered by a future OCCT-enabled build gate.

## Platform implications

Linux CI uses the system C++ compiler and CMake without Qt/OCCT for Gate 0. Windows CI uses the
hosted CMake tool and a supported C++ compiler with the same headless option. Qt/OCCT provisioning
and the GUI build remain a separate application-build concern.

## Subsystem effects

- Serialisation: build target only; no file format change.
- Object identity: no change.
- Transactions: no change; not yet implemented.
- Rebuilding: existing tests are registered; algorithms are unchanged.
- Topological references: existing tests are registered; algorithms and persistence are unchanged.

## Reviewable changes

1. Add the build option and conditionally discover GUI/kernel dependencies.
2. Make the OCCT feature subdirectory conditional while preserving its enabled path.
3. Register all headless tests consistently and conditionally register the OCCT feature test.
4. Replace direct compiler commands in CI with the CMake/CTest contract on Linux and Windows.
5. Configure, build, test, and inspect the diff. If the local toolchain cannot execute a step, report it.
