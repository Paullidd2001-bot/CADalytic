#pragma once
#include <memory>

namespace cadalytic {

class GeomShape {
public:
    virtual ~GeomShape() = default;

    virtual bool isValid() const = 0;
};

using GeomShapePtr = std::shared_ptr<GeomShape>;

} // namespace cadalytic
