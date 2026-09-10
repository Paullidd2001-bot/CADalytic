add_library(cadalytic_events
    EventBus.cpp
    EventBus.h
    EventTypes.h
)

target_include_directories(cadalytic_events PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)