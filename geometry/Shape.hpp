#pragma once
#include <TopoDS_Shape.hxx>
#include <memory>

class Shape {
public:
    Shape() = default;
    explicit Shape(const TopoDS_Shape& shape);

    const TopoDS_Shape& native() const { return *shape_; }
    bool isValid() const { return !shape_->IsNull(); }

protected:
    std::shared_ptr<TopoDS_Shape> shape_;
};
