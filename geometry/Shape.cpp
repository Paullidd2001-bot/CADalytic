#include "Shape.hpp"

Shape::Shape(const TopoDS_Shape& shape)
    : shape_(std::make_shared<TopoDS_Shape>(shape)) {}
