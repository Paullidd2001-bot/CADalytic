#pragma once
#include "../math/Vector3.h"

namespace cad {

class Plane3D {
public:
    Vector3 normal;   // Must be normalized
    double d;         // Plane equation: n·x + d = 0

    Plane3D() : normal(0,1,0), d(0) {}

    Plane3D(const Vector3& n, double d)
        : normal(n.normalized()), d(d) {}

    Plane3D(const Vector3& point, const Vector3& n)
        : normal(n.normalized()), d(-point.dot(normal)) {}

    // Construct from 3 non-collinear points
    Plane3D(const Vector3& a, const Vector3& b, const Vector3& c);

    // Signed distance from point to plane
    double distanceToPoint(const Vector3& p) const;

    // Project a point onto the plane
    Vector3 projectPoint(const Vector3& p) const;
};

}
