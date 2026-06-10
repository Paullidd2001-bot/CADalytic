#include "Box.hpp"
#include <BRepPrimAPI_MakeBox.hxx>

Box::Box(double x, double y, double z)
    : Shape(BRepPrimAPI_MakeBox(x, y, z).Shape()) {}
