#pragma once
#include "Vector3.h"
#include <limits>

namespace cad {

struct Bounds {
    Vector3 min;
    Vector3 max;

    Bounds()
        : min( std::numeric_limits<double>::infinity(),
               std::numeric_limits<double>::infinity(),
               std::numeric_limits<double>::infinity() ),
          max( -std::numeric_limits<double>::infinity(),
               -std::numeric_limits<double>::infinity(),
               -std::numeric_limits<double>::infinity() ) {}

    Bounds(const Vector3& min, const Vector3& max)
        : min(min), max(max) {}

    bool isValid() const {
        return min.x <= max.x && min.y <= max.y && min.z <= max.z;
    }

    void expandToInclude(const Vector3& p) {
        if (p.x < min.x) min.x = p.x;
        if (p.y < min.y) min.y = p.y;
        if (p.z < min.z) min.z = p.z;

        if (p.x > max.x) max.x = p.x;
        if (p.y > max.y) max.y = p.y;
        if (p.z > max.z) max.z = p.z;
    }

    void expandToInclude(const Bounds& b) {
        expandToInclude(b.min);
        expandToInclude(b.max);
    }

    Vector3 center() const {
        return { (min.x + max.x) * 0.5,
                 (min.y + max.y) * 0.5,
                 (min.z + max.z) * 0.5 };
    }

    Vector3 size() const {
        return { max.x - min.x,
                 max.y - min.y,
                 max.z - min.z };
    }

    bool contains(const Vector3& p) const {
        return p.x >= min.x && p.x <= max.x &&
               p.y >= min.y && p.y <= max.y &&
               p.z >= min.z && p.z <= max.z;
    }
};

}
