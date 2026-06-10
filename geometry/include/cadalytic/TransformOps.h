#pragma once
#include "GeomShape.h"
#include "Result.h"

namespace cadalytic {

class TransformOps {
public:
    static Result<GeomShapePtr> translate(const GeomShapePtr& shape, double x, double y, double z);
    static Result<GeomShapePtr> rotate(const GeomShapePtr& shape, double angleDeg, double ax, double ay, double az);
    static Result<GeomShapePtr> scale(const GeomShapePtr& shape, double factor);
};

} // namespace cadalytic
