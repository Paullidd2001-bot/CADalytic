add_library(cadalytic_core
    document/Document.cpp
    document/Document.h
    document/Part.cpp
    document/Part.h
    constraint/Constraint.cpp
    constraint/Constraint.h
    dependency/DependencyGraph.cpp
    dependency/DependencyGraph.h
    feature/Feature.cpp
    feature/Feature.h
    rebuild/RebuildEngine.cpp
    rebuild/RebuildEngine.h
    rebuild/TopologicalNaming.cpp
    rebuild/TopologicalNaming.h
    rebuild/RebuildSafeReferences.cpp
    rebuild/RebuildSafeReferences.h
    sketch/Sketch.cpp
    sketch/Sketch.h
)

target_include_directories(cadalytic_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
