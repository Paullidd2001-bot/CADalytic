#pragma once

#include <cstddef>

#include "../../core/constraint/Constraint.h"

namespace cadalytic {

// Shared spec for the sketch solver. This is the single source of truth for
// how many scalar equations each constraint type contributes, so the numeric
// engine (SketchSolver) and the degrees-of-freedom analysis (DOFAnalyzer)
// can never drift apart.
//
//   Coincident  -> 2  (p1 == p2 in x and y)
//   Horizontal  -> 1  (p1.y == p2.y)
//   Vertical    -> 1  (p1.x == p2.x)
//   Distance    -> 1  (|p1 - p2| == value)
//   Angle       -> 1  (angle(line2) - angle(line1) == value, radians)
std::size_t constraintEquationCount(ConstraintType type);

} // namespace cadalytic