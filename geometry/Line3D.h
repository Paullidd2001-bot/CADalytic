#pragma once
#include "../math/Vector3.h"

namespace cad {

class Line3D {
public:
    Vector3 p0;   // Start point
    Vector3 p1;   // End point

    Line3D() : p0(0,0,0), p1(0,0,0) {}
    Line3D(const Vector3& a, const Vector3& b) : p0(a), p1(b) {}

    Vector3 direction() const {
        return (p1 - p0).normalized();
    }

    double length() const {
        return (p1 - p0).length();
    }

    // Parametric point: L(t) = p0 + t*(p1 - p0)
    Vector3 pointAt(double t) const {
        return p0 + (p1 - p0) * t;
    }

    // Closest point on the line segment to a point
    Vector3 closestPoint(const Vector3& p) const;

    // Distance from point to line segment
    double distanceToPoint(const Vector3& p) const;
};

}
