cmake_minimum_required(VERSION 3.16)
project(CADalytic)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 20)

# ---------------------------------------------------------
# Qt6
# ---------------------------------------------------------
set(Qt6_DIR "C:/msys64/clang64/lib/cmake/Qt6")
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

# ---------------------------------------------------------
# OpenCASCADE
# ---------------------------------------------------------
set(OpenCASCADE_DIR "C:/dev/occt-8.0.0/OCCT-8_0_0/build/install/cmake")
find_package(OpenCASCADE REQUIRED)

# ---------------------------------------------------------
# ANGLE include directories
# ---------------------------------------------------------
include_directories(
    C:/dev/angle-sdk/include
    C:/dev/angle-sdk/include/EGL
    C:/dev/angle-sdk/include/GLES2
)

# ---------------------------------------------------------
# Math module
# ---------------------------------------------------------
add_library(cadalytic_math INTERFACE)
target_include_directories(cadalytic_math INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/math
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
