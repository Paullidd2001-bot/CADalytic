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
    
    // Move semantics
    Matrix4(Matrix4&&) = default;
    Matrix4& operator=(Matrix4&&) = default;
    Matrix4(const Matrix4&) = default;
    Matrix4& operator=(const Matrix4&) = default;

    static inline Matrix4 identity() {
        Matrix4 r;
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0;
        return r;
    }

    static inline Matrix4 translation(const Vector3& t) {
        Matrix4 r = identity();
        r.m[12] = t.x;
        r.m[13] = t.y;
        r.m[14] = t.z;
        return r;
    }

    static inline Matrix4 scale(double s) {
        Matrix4 r = identity();
        r.m[0] = r.m[5] = r.m[10] = s;
        return r;
    }

    static inline Matrix4 fromQuaternion(const Quaternion& q) {
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

    // Optimized matrix multiplication - loop unrolled for better cache locality
    inline Matrix4 operator*(const Matrix4& o) const {
        Matrix4 r;
        // Unroll inner loop for better compiler optimization
        for (int col = 0; col < 4; col++) {
            const double* ocol = &o.m[col * 4];
            const double m0 = m[0], m1 = m[1], m2 = m[2], m3 = m[3];
            const double m4 = m[4], m5 = m[5], m6 = m[6], m7 = m[7];
            const double m8 = m[8], m9 = m[9], m10 = m[10], m11 = m[11];
            const double m12 = m[12], m13 = m[13], m14 = m[14], m15 = m[15];
            
            r.m[col * 4 + 0] = m0 * ocol[0] + m4 * ocol[1] + m8  * ocol[2] + m12 * ocol[3];
            r.m[col * 4 + 1] = m1 * ocol[0] + m5 * ocol[1] + m9  * ocol[2] + m13 * ocol[3];
            r.m[col * 4 + 2] = m2 * ocol[0] + m6 * ocol[1] + m10 * ocol[2] + m14 * ocol[3];
            r.m[col * 4 + 3] = m3 * ocol[0] + m7 * ocol[1] + m11 * ocol[2] + m15 * ocol[3];
        }
        return r;
    }

    inline Vector3 transformPoint(const Vector3& v) const {
        return {
            m[0] * v.x + m[4] * v.y + m[8]  * v.z + m[12],
            m[1] * v.x + m[5] * v.y + m[9]  * v.z + m[13],
            m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14]
        };
    }
};

}
