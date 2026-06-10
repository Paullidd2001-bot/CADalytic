#pragma once
#include "GeomShape.h"

namespace cadalytic {

class GeomVertex : public GeomShape {
public:
    virtual ~GeomVertex() = default;
};

using GeomVertexPtr = std::shared_ptr<GeomVertex>;

} // namespace cadalytic
