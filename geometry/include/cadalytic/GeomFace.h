#pragma once
#include "GeomShape.h"

namespace cadalytic {

class GeomFace : public GeomShape {
public:
    virtual ~GeomFace() = default;
};

using GeomFacePtr = std::shared_ptr<GeomFace>;

} // namespace cadalytic
