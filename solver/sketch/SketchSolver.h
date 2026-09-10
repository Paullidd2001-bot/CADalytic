#pragma once

#include <cstddef>
#include <string>

#include "../../core/sketch/Sketch.h"

namespace cadalytic {

struct SolveResult
{
    bool success = false;
    std::size_t iterations = 0;
    double maxResidual = 0.0;
    std::string error;
};

// Component O: 2D sketch constraint solver.
//
// The solver collects the coordinates of every non-fixed sketch point into a
// single parameter vector, builds one residual per constraint equation, and
// runs a damped Gauss-Newton iteration against a numerically-differentiated
// Jacobian. Because the Jacobian is numeric, adding a new constraint type only
// requires teaching the residual builder about it - nothing else changes.
//
// On success the solved positions are committed back to the sketch. On failure
// the sketch is left untouched.
class SketchSolver
{
public:
    SketchSolver(std::size_t maxIterations = 200, double tolerance = 1e-9);

    SolveResult solve(Sketch& sketch) const;

private:
    std::size_t m_maxIterations;
    double m_tolerance;
};

} // namespace cadalytic