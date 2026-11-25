cmake_minimum_required(VERSION 3.16)
project(ChessEngine LANGUAGES CXX)

## =========================================================
## Global Compiler Settings
## =========================================================

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /permissive- /wd4100)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
endif()

option(ENABLE_FATAL_WARNINGS "Treat warnings as errors" ON)
if (ENABLE_FATAL_WARNINGS)
    if (MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror)
    endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core)

## =========================================================
## Include Directories (global)
## =========================================================

include_directories(include)

## =========================================================
## Source Groups
## =========================================================

set(ENGINE_SOURCES
    src/engine/board.cpp
    src/engine/FEN.cpp
    src/engine/game.cpp
    src/engine/move_generator.cpp
    src/engine/notation.cpp
)

set(UI_SOURCES
    src/ui/cli_ui.cpp
)

set(GUI_QT_SOURCES
    src/ui/gui_qt/qt_main.cpp
    src/ui/gui_qt/windows/ChessWindow.cpp
    src/ui/gui_qt/widgets/BoardWidget.cpp
    src/ui/gui_qt/adapters/engine_adapter.cpp
)

set(TOOLS_SOURCES
    src/tools/perft.cpp
    src/tools/legal_move_FEN_analysis.cpp
)

set(AGENT_SOURCES
    src/agents/Agent_01.cpp
    src/agents/Agent_02.cpp
    src/agents/Agent_03.cpp
    src/agents/Agent_04.cpp
    src/agents/Agent_05.cpp
    src/agents/Agent_06.cpp
)

## =========================================================
## Engine Library
## =========================================================

add_library(chess_engine STATIC
    ${ENGINE_SOURCES}
    ${TOOLS_SOURCES}
    ${AGENT_SOURCES}
)

target_include_directories(chess_engine PUBLIC include)

## =========================================================
## Executable: Chess (CLI)
## =========================================================

add_executable(Chess
    src/main.cpp
    ${UI_SOURCES}
)

target_link_libraries(Chess PRIVATE chess_engine)

## =========================================================
## Executable: Perft Tool
## =========================================================

add_executable(perft
    src/perft_main.cpp
)

target_link_libraries(perft PRIVATE chess_engine)

## =========================================================
## Executable: Legal Move FEN Analysis Tool
## =========================================================

add_executable(legalmoves
    src/legal_move_FEN_analysis_main.cpp
)

target_link_libraries(legalmoves PRIVATE chess_engine)

## =========================================================
## Executable: Agent Tournament
## =========================================================

add_executable(agent_tournament
    src/tournament/agent_tournament.cpp
)

target_link_libraries(agent_tournament PRIVATE chess_engine)

# =========================================================
# Executable: Qt GUI
# =========================================================

add_executable(chess_gui
    ${GUI_QT_SOURCES}
)

qt_add_resources(GUI_RESOURCES
    src/ui/gui_qt/resources/chess_resources.qrc
)

target_sources(chess_gui PRIVATE
    ${GUI_RESOURCES}
    # Add headers explicitly so AUTOMOC processes Q_OBJECT
    include/ui/gui_qt/widgets/BoardWidget.h
    include/ui/gui_qt/windows/ChessWindow.h
)

target_include_directories(chess_gui PRIVATE
    include               # your headers
)

target_link_libraries(chess_gui
    chess_engine          # your engine library
    Qt6::Widgets
    Qt6::Gui
    Qt6::Core
)

## =========================================================
## Unit Testing (Catch2)
## =========================================================

enable_testing()

file(GLOB TEST_SOURCES tests/*.cpp)

add_executable(chess_tests
    ${TEST_SOURCES}
    tests/Catch2/catch_amalgamated.cpp
)

target_include_directories(chess_tests PRIVATE
    include
    tests/Catch2
)

target_link_libraries(chess_tests PRIVATE chess_engine)

add_test(NAME chess_tests COMMAND chess_tests)

## =========================================================
## Output message
## =========================================================

message(STATUS "ChessEngine configured successfully.")
