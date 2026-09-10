add_library(cadalytic_solver
    sketch/SketchConstraints.cpp
    sketch/SketchConstraints.h
    sketch/SketchSolver.cpp
    sketch/SketchSolver.h
    sketch/DOFAnalyzer.cpp
    sketch/DOFAnalyzer.h
)

target_include_directories(cadalytic_solver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(cadalytic_solver PUBLIC
    cadalytic_core
)