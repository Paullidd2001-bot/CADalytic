cmake_minimum_required(VERSION 3.16)
project(CADalytic)

include(CTest)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 20)

# Enable optimizations
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL")
endif()

# ---------------------------------------------------------
# Qt6 - Use find_package for cross-platform compatibility
# ---------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

# ---------------------------------------------------------
# OpenCASCADE - Use find_package for portability
# ---------------------------------------------------------
find_package(OpenCASCADE REQUIRED)

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
    geometry
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    ${OpenCASCADE_LIBRARIES}
)

# Enable Link-Time Optimization for Release builds
if(MSVC)
    target_link_options(cadalytic PRIVATE $<$<CONFIG:Release>:/LTCG>)
else()
    target_compile_options(cadalytic PRIVATE $<$<CONFIG:Release>:-flto>)
    target_link_options(cadalytic PRIVATE $<$<CONFIG:Release>:-flto>)
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
endif()
