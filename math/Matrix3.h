#pragma once
#include "Vector3.h"
#include <cmath>

namespace cad {

struct Matrix3 {
    // Column-major storage: m[col * 3 + row]
    double m[9];

    Matrix3() {
        for (int i = 0; i < 9; i++) m[i] = 0.0;
    }

    static Matrix3 identity() {
        Matrix3 r;
        r.m[0] = r.m[4] = r.m[8] = 1.0;
        return r;
    }

    Vector3 operator*(const Vector3& v) const {
        return {
            m[0] * v.x + m[3] * v.y + m[6] * v.z,
            m[1] * v.x + m[4] * v.y + m[7] * v.z,
            m[2] * v.x + m[5] * v.y + m[8] * v.z
        };
    }

    Matrix3 operator*(const Matrix3& o) const {
        Matrix3 r;
        for (int col = 0; col < 3; col++) {
            for (int row = 0; row < 3; row++) {
                r.m[col * 3 + row] =
                    m[0 * 3 + row] * o.m[col * 3 + 0] +
                    m[1 * 3 + row] * o.m[col * 3 + 1] +
                    m[2 * 3 + row] * o.m[col * 3 + 2];
            }
        }
        return r;
    }
};

}
