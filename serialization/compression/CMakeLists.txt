# Component M: dependency-free compression layer for the native file format.
add_library(cadalytic_compression
    Compression.cpp
    Compression.h
)

target_include_directories(cadalytic_compression PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)