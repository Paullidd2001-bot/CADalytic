#pragma once
#include "GeomShape.h"

namespace cadalytic {

class GeomEdge : public GeomShape {
public:
    virtual ~GeomEdge() = default;
};

using GeomEdgePtr = std::shared_ptr<GeomEdge>;

} // namespace cadalytic
