# Units and Tolerances

The model uses SI base units internally: length in metres, mass in kilograms, time in seconds, and angles in radians. Display units are presentation preferences and are converted at boundaries. File persistence stores canonical internal values plus dimension metadata where needed.

Every dimensional property declares its dimension. Plain `double` is acceptable only for dimensionless values or temporary adapter data. Degrees may be accepted at user/API boundaries but are converted immediately to radians.

Geometric comparisons use documented absolute and relative tolerances. The policy must be configurable per operation but deterministic within a rebuild. Equality of shapes is never inferred from face or edge counts alone; validity, topology, bounding box, and relevant mass properties are considered together.
