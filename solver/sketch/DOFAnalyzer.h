#pragma once

#include <cstddef>
#include <string>

#include "../../core/sketch/Sketch.h"

namespace cadalytic {

struct DOFReport
{
    std::size_t parameterCount = 0; // 2 per non-fixed point
    std::size_t equationCount = 0;  // sum of constraint equations
    std::size_t pointCount = 0;
    std::size_t fixedPointCount = 0;
    std::size_t componentCount = 0; // connected components of the constraint graph
    std::size_t degreesOfFreedom = 0;
    bool overconstrained = false;
    bool fullyConstrained = false;
    bool underconstrained = false;
    bool malformed = false; // a constraint references missing geometry
    std::string detail;
};

// Component O: degrees-of-freedom analysis for a 2D sketch.
//
// Parameters are the x/y coordinates of every non-fixed point; every
// constraint contributes a fixed number of equations (see
// constraintEquationCount). The constraint graph connects points that are
// bound together by constraints or joined by a line; its connected components
// are reported so callers can detect independent sub-sketches.
class DOFAnalyzer
{
public:
    DOFReport analyze(const Sketch& sketch) const;
};

} // namespace cadalytic