#pragma once
#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix4.h"

namespace cad {

struct Transform {
    Vector3 position;
    Quaternion rotation;
    double scale = 1.0;

    Matrix4 toMatrix() const {
        Matrix4 T = Matrix4::translation(position);
        Matrix4 R = Matrix4::fromQuaternion(rotation);
        Matrix4 S = Matrix4::scale(scale);
        return T * R * S;
    }
};

}

