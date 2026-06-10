#pragma once
#include <cmath>
#include "Vector3.h"
#include "Quaternion.h"

namespace cad {

struct Matrix4 {
    // Column-major storage: m[col * 4 + row]
    double m[16];

    Matrix4() {
        for (int i = 0; i < 16; i++) m[i] = 0.0;
    }

    static Matrix4 identity() {
        Matrix4 r;
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0;
        return r;
    }

    static Matrix4 translation(const Vector3& t) {
        Matrix4 r = identity();
        r.m[12] = t.x;
        r.m[13] = t.y;
        r.m[14] = t.z;
        return r;
    }

    static Matrix4 scale(double s) {
        Matrix4 r = identity();
        r.m[0] = r.m[5] = r.m[10] = s;
        return r;
    }

    static Matrix4 fromQuaternion(const Quaternion& q) {
        Matrix4 r = identity();

        double xx = q.x * q.x;
        double yy = q.y * q.y;
        double zz = q.z * q.z;
        double xy = q.x * q.y;
        double xz = q.x * q.z;
        double yz = q.y * q.z;
        double wx = q.w * q.x;
        double wy = q.w * q.y;
        double wz = q.w * q.z;

        r.m[0] = 1 - 2 * (yy + zz);
        r.m[1] =     2 * (xy + wz);
        r.m[2] =     2 * (xz - wy);

        r.m[4] =     2 * (xy - wz);
        r.m[5] = 1 - 2 * (xx + zz);
        r.m[6] =     2 * (yz + wx);

        r.m[8]  =    2 * (xz + wy);
        r.m[9]  =    2 * (yz - wx);
        r.m[10] = 1 - 2 * (xx + yy);

        return r;
    }

    Matrix4 operator*(const Matrix4& o) const {
        Matrix4 r;
        for (int col = 0; col < 4; col++) {
            for (int row = 0; row < 4; row++) {
                r.m[col * 4 + row] =
                    m[0 * 4 + row] * o.m[col * 4 + 0] +
                    m[1 * 4 + row] * o.m[col * 4 + 1] +
                    m[2 * 4 + row] * o.m[col * 4 + 2] +
                    m[3 * 4 + row] * o.m[col * 4 + 3];
            }
        }
        return r;
    }

    Vector3 transformPoint(const Vector3& v) const {
        return {
            m[0] * v.x + m[4] * v.y + m[8]  * v.z + m[12],
            m[1] * v.x + m[5] * v.y + m[9]  * v.z + m[13],
            m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14]
        };
    }
};

}