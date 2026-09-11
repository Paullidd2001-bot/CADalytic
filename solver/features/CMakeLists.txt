# Component R: 3D feature solver. This library touches OCCT directly, so it
# sits apart from the sketch-only `cadalytic_solver` target and is only built
# where OpenCASCADE is available (the top-level CMake requires it).
add_library(cadalytic_features
    FeatureSolver.cpp
    FeatureSolver.h
)

target_include_directories(cadalytic_features PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(cadalytic_features PUBLIC
    cadalytic_solver
    cadalytic_core
    geometry
    ${CADALYTIC_OCCT_LIBRARIES}
)

target_include_directories(cadalytic_features PUBLIC
    ${OpenCASCADE_INCLUDE_DIRS}
)