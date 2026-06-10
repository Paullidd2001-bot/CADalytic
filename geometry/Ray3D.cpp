#include "Ray3D.h"
#include <algorithm>

namespace cad {

double Ray3D::projectPoint(const Vector3& p) const {
    Vector3 op = p - origin;
    return op.dot(direction);   // t value (can be negative)
}

double Ray3D::distanceToPoint(const Vector3& p) const {
    double t = projectPoint(p);

    // Closest point on infinite ray
    if (t < 0.0)
        return (p - origin).length();

    Vector3 closest = origin + direction * t;
    return (p - closest).length();
}

}
