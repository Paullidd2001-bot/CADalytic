add_library(geometry
    Shape.cpp
    Box.cpp
    Cylinder.cpp
    Line3D.h
    Line3D.cpp
    Plane3D.h
    Plane3D.cpp
    Ray3D.h
    Ray3D.cpp
)

# Geometry needs OCCT headers
target_include_directories(geometry PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCASCADE_INCLUDE_DIRS}
)

# Geometry needs OCCT libraries
target_link_libraries(geometry PUBLIC
    TKernel
    TKMath
    TKBRep
    TKGeomBase
    TKPrim
)
