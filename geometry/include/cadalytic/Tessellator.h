#pragma once
#include "GeomShape.h"
#include <vector>

namespace cadalytic {

struct Mesh {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

class Tessellator {
public:
    static Mesh tessellate(const GeomShapePtr& shape, double tolerance = 0.1);
};

} // namespace cadalytic
