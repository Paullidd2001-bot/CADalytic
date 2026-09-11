cmake_minimum_required(VERSION 3.16)
project(CADalytic)

include(CTest)

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_CXX_STANDARD 20)

# Enable optimizations
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL")
endif()

# ---------------------------------------------------------
# OpenCASCADE - Use find_package for portability
# ---------------------------------------------------------
set(CADALYTIC_OCCT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/occt/usr"
    CACHE PATH "OpenCASCADE installation prefix")

if(EXISTS "${CADALYTIC_OCCT_ROOT}/lib/x86_64-linux-gnu/cmake/opencascade/OpenCASCADEConfig.cmake")
    list(PREPEND CMAKE_PREFIX_PATH "${CADALYTIC_OCCT_ROOT}")
endif()

find_package(OpenCASCADE REQUIRED CONFIG COMPONENTS
    FoundationClasses
    ModelingData
    ModelingAlgorithms
)

set(CADALYTIC_OCCT_LIBRARIES
    TKernel
    TKMath
    TKG2d
    TKG3d
    TKGeomBase
    TKBRep
    TKGeomAlgo
    TKTopAlgo
    TKPrim
    TKBO
    TKShHealing
    TKBool
    TKFillet
    TKOffset
    TKFeat
)

if(EXISTS "${CADALYTIC_OCCT_ROOT}/lib/x86_64-linux-gnu")
    list(APPEND CMAKE_BUILD_RPATH
        "${CADALYTIC_OCCT_ROOT}/lib/x86_64-linux-gnu")
endif()

# This vendored Debian package was built with optional TBB support, but its
# imported targets retain references to development symlinks not shipped here.
foreach(CADALYTIC_OCCT_TARGET IN LISTS CADALYTIC_OCCT_LIBRARIES)
    if(TARGET ${CADALYTIC_OCCT_TARGET})
        get_target_property(CADALYTIC_OCCT_LINKS
            ${CADALYTIC_OCCT_TARGET} INTERFACE_LINK_LIBRARIES)
        if(CADALYTIC_OCCT_LINKS)
            list(FILTER CADALYTIC_OCCT_LINKS EXCLUDE REGEX "libtbb")
            set_target_properties(${CADALYTIC_OCCT_TARGET} PROPERTIES
                INTERFACE_LINK_LIBRARIES "${CADALYTIC_OCCT_LINKS}")
        endif()
    endif()
endforeach()

# The Debian-generated package config stores its original system include path;
# use the actual prefix when the package is vendored with the project.
if(EXISTS "${CADALYTIC_OCCT_ROOT}/include/opencascade")
    set(OpenCASCADE_INCLUDE_DIRS "${CADALYTIC_OCCT_ROOT}/include/opencascade")
endif()

# ---------------------------------------------------------
# Qt6 - Use find_package for cross-platform compatibility
# ---------------------------------------------------------
find_package(Qt6 QUIET COMPONENTS Widgets OpenGL OpenGLWidgets)
if(Qt6_FOUND)
    set(CADALYTIC_BUILD_GUI ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
else()
    set(CADALYTIC_BUILD_GUI OFF)
    message(WARNING "Qt6 was not found; skipping the CADalytic GUI executable")
endif()

# ---------------------------------------------------------
# Math module - Header-only with optimizations
# ---------------------------------------------------------
add_library(cadalytic_math INTERFACE)
target_include_directories(cadalytic_math INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/math
)
# Enable MSVC inline optimization
target_compile_options(cadalytic_math INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/Ob2>
)

# ---------------------------------------------------------
# Geometry module
# ---------------------------------------------------------
add_subdirectory(geometry)
target_include_directories(geometry PUBLIC
    ${OpenCASCADE_INCLUDE_DIRS}
)

# ---------------------------------------------------------
# Core document model
# ---------------------------------------------------------
add_subdirectory(core)

# ---------------------------------------------------------
# Serialization module
# ---------------------------------------------------------
add_subdirectory(serialization)

# ---------------------------------------------------------
# Solver module (Component O)
# ---------------------------------------------------------
add_subdirectory(solver)

# ---------------------------------------------------------
# Event system (Component J)
# ---------------------------------------------------------
add_subdirectory(events)

# ---------------------------------------------------------
# Command system (Component I)
# ---------------------------------------------------------
add_subdirectory(command)

if(CADALYTIC_BUILD_GUI)
    # ---------------------------------------------------------
    # Main executable
    # ---------------------------------------------------------
    add_executable(cadalytic
        src/main.cpp
        src/MainWindow.cpp
        src/MainWindow.h
        src/Occt3DView.cpp
        src/Occt3DView.h
    )

    target_include_directories(cadalytic PRIVATE
        ${OpenCASCADE_INCLUDE_DIRS}
    )

    # ---------------------------------------------------------
    # Link everything
    # ---------------------------------------------------------
    target_link_libraries(cadalytic PRIVATE
        cadalytic_math
        cadalytic_core
        cadalytic_features
        cadalytic_serialization
        geometry
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        ${CADALYTIC_OCCT_LIBRARIES}
    )

    # Enable Link-Time Optimization for Release builds
    if(MSVC)
        target_link_options(cadalytic PRIVATE $<$<CONFIG:Release>:/LTCG>)
    else()
        target_compile_options(cadalytic PRIVATE $<$<CONFIG:Release>:-flto>)
        target_link_options(cadalytic PRIVATE $<$<CONFIG:Release>:-flto>)
    endif()
endif()

if(BUILD_TESTING)
    add_executable(cadalytic_core_tests
        tests/core_model_tests.cpp
    )
    target_link_libraries(cadalytic_core_tests PRIVATE cadalytic_core)
    add_test(NAME cadalytic_core_tests COMMAND cadalytic_core_tests)

    add_executable(cadalytic_rebuild_tests
        tests/rebuild_system_tests.cpp
    )
    target_link_libraries(cadalytic_rebuild_tests PRIVATE cadalytic_core)
    add_test(NAME cadalytic_rebuild_tests COMMAND cadalytic_rebuild_tests)

    add_executable(cadalytic_solver_tests
        tests/solver_tests.cpp
    )
    target_link_libraries(cadalytic_solver_tests PRIVATE cadalytic_solver cadalytic_core)
    add_test(NAME cadalytic_solver_tests COMMAND cadalytic_solver_tests)

    add_executable(cadalytic_event_tests
        tests/event_tests.cpp
    )
    target_link_libraries(cadalytic_event_tests PRIVATE cadalytic_events)
    add_test(NAME cadalytic_event_tests COMMAND cadalytic_event_tests)

    add_executable(cadalytic_command_tests
        tests/command_tests.cpp
    )
    target_link_libraries(cadalytic_command_tests PRIVATE cadalytic_command cadalytic_events cadalytic_core)
    add_test(NAME cadalytic_command_tests COMMAND cadalytic_command_tests)

    add_executable(cadalytic_serialization_tests
        tests/serialization_tests.cpp
    )
    target_link_libraries(cadalytic_serialization_tests PRIVATE cadalytic_serialization cadalytic_core)
    add_test(NAME cadalytic_serialization_tests COMMAND cadalytic_serialization_tests)

    # Component R: needs OpenCASCADE (already required at the top level).
    add_executable(cadalytic_feature_solver_tests
        tests/feature_solver_tests.cpp
    )
    target_link_libraries(cadalytic_feature_solver_tests PRIVATE
        cadalytic_features cadalytic_solver cadalytic_core)
    add_test(NAME cadalytic_feature_solver_tests COMMAND cadalytic_feature_solver_tests)
endif()
