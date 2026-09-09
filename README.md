# ChessEngine

A work-in-progress chess move-generation engine written in modern C++.

The project is focused on building a correct and well-tested chess core before adding search and a UCI interface. The current executable is a small development harness that runs a perft divide on the Kiwipete position.

## Current features

- C++23 implementation with a CMake/Ninja build
- Bitboard-based board representation with a mailbox for direct square access
- FEN parsing and serialization with structured validation errors
- Reversible move execution with `do_move` / `undo_move`
- Pseudo-legal and legal move generation
- Attack detection for all pieces
- Special moves:
  - castling
  - en passant
  - promotions to knight, bishop, rook, and queen
- Perft and perft divide utilities
- Catch2 unit tests covering board updates, FEN handling, move legality, and known perft positions
- Separate Debug and Release CMake presets for Linux and Windows MinGW

## Project status

This is an experimental engine project and is still under active development. It does not have a chess search, evaluation function, or UCI command loop yet. The `ChessEngine` executable currently runs the built-in perft example from `src/main.cpp`.

Planned next steps include:

- adding a UCI interface
- implementing position evaluation and search
- improving move-generation performance
- adding benchmarks and engine-level tests

## Requirements

- A C++23-compatible compiler
- CMake 3.30 or newer
- Ninja
- [vcpkg](https://github.com/microsoft/vcpkg)

The vcpkg manifest provides the project dependencies:

- [Catch2](https://github.com/catchorg/Catch2) for tests
- [Google Benchmark](https://github.com/google/benchmark) for the optional benchmark target

## Build on Linux

Install the prerequisites, then configure and build the Debug preset:

```bash
git clone https://github.com/Malugeli/Chess-Engine-2.0.git
cd Chess

export VCPKG_ROOT=/path/to/vcpkg
vcpkg install --triplet x64-linux

cmake --preset linux-debug
cmake --build --preset linux-debug
```

The executable is created at `build/Debug/ChessEngine`.

To use the Release preset instead:

```bash
cmake --preset linux-release
cmake --build --preset linux-release
```

## Build on Windows

The Windows presets use GCC/MinGW and the static `x64-mingw-static` vcpkg triplet:

```powershell
git clone https://github.com/Malugeli/Chess-Engine-2.0.git
cd Chess

$env:VCPKG_ROOT = "C:\path\to\vcpkg"
vcpkg install --triplet x64-mingw-static

cmake --preset windows-debug
cmake --build --preset windows-debug
```

The executable is created at `build\Debug\ChessEngine.exe`.

## Run the tests

CTest discovers the individual Catch2 test cases through CMake:

```bash
# Linux
ctest --preset linux-debug

# Windows
ctest --preset windows-debug
```

The test suite contains fast correctness tests and slower deeper perft checks. To run only the fast tests from a Debug build:

```bash
ctest --test-dir build/Debug -L fast
```

The perft tests validate the move generator against standard reference positions, including the starting position and Kiwipete.

## Optional benchmarks

Google Benchmark is already declared in the vcpkg manifest. The CMake option
`CHESS_BUILD_BENCH` is reserved for the benchmark target and remains disabled
until benchmark sources are added to the repository.

## Repository layout

```text
src/
├── board.*       Board representation, FEN, and reversible moves
├── movegen.*     Attack generation and pseudo-legal/legal moves
├── perft.*       Move-tree counting and perft divide output
├── types.hpp     Core chess types and compact move encoding
├── bitboard.hpp  Bitboard helpers and board masks
├── debug.*       Debug and UCI-notation formatting helpers
└── main.cpp      Current development executable

tests/
├── board.cpp     Move execution and undo tests
├── fen.cpp       FEN validation and round-trip tests
├── movegen.cpp   Legal move-generation tests
└── perft.cpp     Reference perft tests
```

## License

No license has been added yet. Until a license is provided, the code should be considered “all rights reserved.”
