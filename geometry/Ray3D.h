#pragma once
#include "../math/Vector3.h"

namespace cad {

class Ray3D {
public:
    Vector3 origin;
    Vector3 direction;   // Must be normalized

    Ray3D()
        : origin(0,0,0), direction(0,0,1) {}

    Ray3D(const Vector3& o, const Vector3& d)
        : origin(o), direction(d.normalized()) {}

    // Evaluate point along ray: R(t) = origin + t * direction
    Vector3 pointAt(double t) const {
        return origin + direction * t;
    }

    // Distance from point to infinite ray
    double distanceToPoint(const Vector3& p) const;

    // Project point onto ray (returns t parameter)
    double projectPoint(const Vector3& p) const;
};

}
