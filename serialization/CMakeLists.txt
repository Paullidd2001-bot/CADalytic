add_library(cadalytic_serialization
    DocumentSerializer.cpp
    DocumentSerializer.h
)

target_include_directories(cadalytic_serialization PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(cadalytic_serialization PUBLIC
    cadalytic_core
)
