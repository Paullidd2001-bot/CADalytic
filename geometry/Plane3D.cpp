#include "Plane3D.h"

namespace cad {

Plane3D::Plane3D(const Vector3& a, const Vector3& b, const Vector3& c) {
    Vector3 ab = b - a;
    Vector3 ac = c - a;
    normal = ab.cross(ac).normalized();
    d = -a.dot(normal);
}

double Plane3D::distanceToPoint(const Vector3& p) const {
    return normal.dot(p) + d;
}

Vector3 Plane3D::projectPoint(const Vector3& p) const {
    double dist = distanceToPoint(p);
    return p - normal * dist;
}

}
