#include "Line3D.h"
#include <algorithm>

namespace cad {

Vector3 Line3D::closestPoint(const Vector3& p) const {
    Vector3 d = p1 - p0;
    double len2 = d.dot(d);

    if (len2 == 0.0)
        return p0; // Degenerate line

    double t = (p - p0).dot(d) / len2;
    t = std::clamp(t, 0.0, 1.0);

    return p0 + d * t;
}

double Line3D::distanceToPoint(const Vector3& p) const {
    Vector3 c = closestPoint(p);
    return (p - c).length();
}

}
