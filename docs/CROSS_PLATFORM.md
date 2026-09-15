# Cross-Platform Support

Windows and Linux are equal target platforms. Current evidence is weaker: Windows is the exercised development environment, while Linux has only a partial Ubuntu workflow and no verified application build in this assessment.

## Required build policy

Use CMake presets or equivalent reproducible configuration. Keep Qt and OCCT discovery in CMake. Do not use absolute paths, registry access, COM, or shell-specific logic in shared modules. Platform services belong behind interfaces. Record compiler, Qt, OCCT, and CMake versions in CI.

## CI progression

Gate 0 requires a headless Linux job and a Windows job. Add the Qt/OCCT application build when dependency provisioning is reproducible. Prefer Clang and GCC on Linux and Microsoft Visual C++ on Windows; add clang-cl only when it provides a demonstrated benefit.

The current workflow is direct compiler smoke testing, is Linux-only, and contains an indentation defect. It is not evidence of cross-platform support.
