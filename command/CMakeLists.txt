add_library(cadalytic_command
    Command.cpp
    Command.h
    CommandManager.cpp
    CommandManager.h
    commands/AddConstraintCommand.h
    commands/AddSketchCommand.h
    commands/AddSketchLineCommand.h
    commands/AddSketchPointCommand.h
    commands/SetConstraintValueCommand.h
    commands/SetSketchPointPositionCommand.h
)

target_include_directories(cadalytic_command PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(cadalytic_command PUBLIC
    cadalytic_core
    cadalytic_events
)