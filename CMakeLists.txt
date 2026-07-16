cmake_minimum_required(VERSION 3.30)

# Hier ist deine Version 2.0
project(ChessEngine CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(benchmark CONFIG REQUIRED)

add_executable(ChessEngine
    src/main.cpp
    src/board.cpp
    src/debug.cpp
    src/movegen.cpp
)

# --- SENIOR DEVELOPER COMPILER WARNINGS ---
# Prüft, welchen Compiler du nutzt (MSVC für Windows/Visual Studio, GCC/Clang für Linux/Mac/Mingw)
if(MSVC)
    target_compile_options(ChessEngine PRIVATE 
        /W4          # Höchstes normales Warnlevel
        /WX          # Behandelt Warnungen als Fehler (Kompilierung bricht ab)
        /permissive- # Erzwingt strikte C++ Standardkonformität
    )
else()
    target_compile_options(ChessEngine PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic 
        -Wconversion      # Extrem wichtig für Schach-Engines!
        -Wsign-conversion # Warnt, wenn signed und unsigned gemischt werden
        -Wshadow          # Warnt, wenn eine lokale Variable eine äußere überdeckt
        -Werror           # Macht jede Warnung zu einem echten Fehler
    )
endif()

target_compile_features(ChessEngine PRIVATE cxx_std_23)
target_link_libraries(ChessEngine PRIVATE benchmark::benchmark)