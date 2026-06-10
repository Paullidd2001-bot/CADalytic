#pragma once
#include <cmath>

namespace cad {

constexpr double PI = 3.14159265358979323846;

constexpr double deg2rad(double d) { return d * PI / 180.0; }
constexpr double rad2deg(double r) { return r * 180.0 / PI; }

}
