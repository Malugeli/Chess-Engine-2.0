cmake_minimum_required(VERSION 3.30)

project(ChessEngine CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Ohne das haengt CMake bei C++23 an jeden g++-Aufruf -fmodules-ts,
# -fmodule-mapper und -fdeps-format an, um nach Modulen zu scannen.
# Wir nutzen keine Module: kostet nur Buildzeit und verwirrt clangd.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

enable_testing()

option(CHESS_BUILD_BENCH "Baut das Benchmark-Target (braucht google/benchmark)" OFF)

# ---------------------------------------------------------------------------
# chess_warnings: kompiliert selbst nichts, ist nur ein Bündel Compilerflags
# mit Namen. Wird an jedes Target gelinkt, das die Flags haben soll — damit
# steht der if(MSVC)-Block genau einmal im File.
# ---------------------------------------------------------------------------
add_library(chess_warnings INTERFACE)

if(MSVC)
    target_compile_options(chess_warnings INTERFACE
        /W4          # Höchstes normales Warnlevel
        /WX          # Behandelt Warnungen als Fehler (Kompilierung bricht ab)
        /permissive- # Erzwingt strikte C++ Standardkonformität
    )
else()
    target_compile_options(chess_warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion      # Extrem wichtig für Schach-Engines!
        -Wsign-conversion # Warnt, wenn signed und unsigned gemischt werden
        -Wshadow          # Warnt, wenn eine lokale Variable eine äußere überdeckt
        -Werror           # Macht jede Warnung zu einem echten Fehler
    )
endif()

# ---------------------------------------------------------------------------
# chess_core: die Engine als Bibliothek. Enthält KEIN main() — sonst würde
# es mit dem main() aus Catch2 kollidieren. Jede Quelldatei steht genau hier
# und nirgends sonst.
# ---------------------------------------------------------------------------
add_library(chess_core STATIC
    src/board.cpp
    src/debug.cpp
    src/movegen.cpp
    src/perft.cpp
)

# PUBLIC = wer gegen chess_core linkt, erbt src/ im Include-Pfad.
# Deshalb funktioniert #include "board.hpp" auch aus tests/ heraus.
target_include_directories(chess_core PUBLIC src)
target_compile_features(chess_core PUBLIC cxx_std_23)

# PRIVATE: die harten Warnungen gelten für den Engine-Code, aber sie werden
# nicht an die Tests weitervererbt (Catch2-Makros mögen -Wconversion nicht).
target_link_libraries(chess_core PRIVATE chess_warnings)

# std::print/std::println liegen bei GCC 14/15 in libstdc++exp, nicht in libstdc++,
# weil ihr ABI noch nicht stabil ist. Ohne das fehlen __open_terminal/__write_to_terminal.
# PUBLIC, damit ChessEngine und chess_tests das ebenfalls mitbekommen.
target_link_libraries(chess_core PUBLIC $<$<CXX_COMPILER_ID:GNU>:stdc++exp>)

# ---------------------------------------------------------------------------
# ChessEngine: das eigentliche Programm. Wird später die UCI-Schleife.
# ---------------------------------------------------------------------------
add_executable(ChessEngine src/main.cpp)
target_link_libraries(ChessEngine PRIVATE chess_core chess_warnings)

# ---------------------------------------------------------------------------
# chess_tests: Catch2 liefert das main(), deine Testdateien enthalten nur
# TEST_CASEs. Neue Testdatei = eine Zeile hier ergänzen.
# ---------------------------------------------------------------------------
find_package(Catch2 3 CONFIG REQUIRED)

add_executable(chess_tests
    tests/smoke.cpp
    tests/board.cpp
    tests/fen.cpp
    tests/perft.cpp
)
target_link_libraries(chess_tests PRIVATE chess_core Catch2::Catch2WithMain)

# catch_discover_tests ruft die fertige .exe auf, fragt sie nach ihren
# TEST_CASEs und registriert jeden einzeln bei CTest. Dadurch zeigt ctest
# "FEN round-trip ... Failed" statt nur "chess_tests ist rot".
include(Catch)
catch_discover_tests(chess_tests ADD_TAGS_AS_LABELS)

# ---------------------------------------------------------------------------
# Benchmarks: erst relevant, wenn Perft läuft und du Magic Bitboards misst.
# Einschalten mit -DCHESS_BUILD_BENCH=ON, sobald es bench/ gibt.
# ---------------------------------------------------------------------------
if(CHESS_BUILD_BENCH)
    find_package(benchmark CONFIG REQUIRED)
    add_executable(chess_bench bench/bench_main.cpp)
    target_link_libraries(chess_bench PRIVATE chess_core chess_warnings benchmark::benchmark)
endif()
