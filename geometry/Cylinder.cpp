#include "Cylinder.hpp"
#include <BRepPrimAPI_MakeCylinder.hxx>

Cylinder::Cylinder(double radius, double height)
    : Shape(BRepPrimAPI_MakeCylinder(radius, height).Shape()) {}
