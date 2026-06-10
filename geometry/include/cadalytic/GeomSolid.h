#pragma once
#include "GeomShape.h"

namespace cadalytic {

class GeomSolid : public GeomShape {
public:
    virtual ~GeomSolid() = default;
};

using GeomSolidPtr = std::shared_ptr<GeomSolid>;

} // namespace cadalytic
