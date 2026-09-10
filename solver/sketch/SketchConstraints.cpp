#include "SketchConstraints.h"

namespace cadalytic {

std::size_t constraintEquationCount(ConstraintType type)
{
    switch (type) {
    case ConstraintType::Coincident:
        return 2;
    case ConstraintType::Horizontal:
    case ConstraintType::Vertical:
    case ConstraintType::Distance:
    case ConstraintType::Angle:
        return 1;
    }
    return 0;
}

} // namespace cadalytic