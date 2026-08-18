cmake_minimum_required(VERSION 3.16)
project(CADalytic)

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
# Enable inline optimizations
target_compile_options(cadalytic_math INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/Ob2>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finline-limit=10000>
)

# ---------------------------------------------------------
# Geometry module
# ---------------------------------------------------------
add_subdirectory(geometry)
target_include_directories(geometry PUBLIC
    ${OpenCASCADE_INCLUDE_DIRS}
)

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
