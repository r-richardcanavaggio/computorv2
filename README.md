# computorv2

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://isocpp.org/)
[![Build-Make](https://img.shields.io/badge/build-Make-informational)](Makefile)
[![Build-CMake](https://img.shields.io/badge/build-CMake-informational)](CMakeLists.txt)

computorv2 is an interactive command-line math interpreter written in C++20.
It reads an expression, parses it into an AST, evaluates it in a shared context, and prints the result.

## What The Project Does

computorv2 provides a REPL for symbolic and numeric calculations with support for:

- real numbers
- complex numbers
- polynomials and polynomial functions
- matrices
- variables and function definitions
- built-in math functions (`cos`, `sin`, `tan`, `sqrt`, `abs`, `exp`, `norm`, `inv`)

The runtime is structured as a classic pipeline:

- lexer
- parser (recursive-descent AST)
- interpreter/evaluator

## Why The Project Is Useful

- Mix symbolic and numeric workflows in one terminal session.
- Keep reusable values/functions in context (`list`, `history`, `clear`).
- Work with both scalar and advanced types (complex, polynomial, matrix).
- Plot polynomial functions from the REPL (`plot <func>`).
- Use test fixtures in [tests/inputs](tests/inputs) and [tests/expected](tests/expected) for quick regression checks.

## How To Get Started

### Prerequisites

- C++20 compiler (`c++`, `clang++`, or `g++`)
- `make`
- `cmake` (optional alternative build flow)
- `readline` library
- SFML 2.5.1 (for plotting):
  - with CMake, local SFML is used when found
  - otherwise SFML is fetched automatically by CMake

### Build

Using Make:

```bash
make
```

Using CMake:

```bash
cmake -S . -B build
cmake --build build
```

### Run

Make build output:

```bash
./computorv2
```

CMake build output:

```bash
./build/computorv2
```

### Quick Usage Examples

```text
a = 5
b = -3.2
c = a + b * 2
a = ?
```

```text
f(x) = x^2 + 2x + 1
x = 2
f(x) = ?
```

```text
f(x) = x^2 + 2x + 1
f(x) = 0?
```

### Built-in REPL Commands

- `help`
- `list`
- `history`
- `clear <variable>`
- `plot <function>`
- `deg` / `rad`
- `exit` / `quit`

### Run Tests

```bash
make test
```

or:

```bash
./run_tests.sh
```

Test artifacts:

- expected snapshots: [tests/expected](tests/expected)
- generated outputs: [tests/actual](tests/actual)
- failing diffs: `tests/actual/*.diff`

## Where Users Can Get Help

- Start with REPL command help: run `help` inside computorv2.
- Review practical test scenarios in [tests/inputs](tests/inputs).
- Open a GitHub Issue in this repository for bugs, questions, or feature requests.
- For implementation details, browse:
  - interpreter flow: [srcs/Interpreter](srcs/Interpreter)
  - parser and lexer: [srcs/Parser.cpp](srcs/Parser.cpp), [srcs/Lexer.cpp](srcs/Lexer.cpp)
  - type system: [includes/Types](includes/Types), [srcs/Types](srcs/Types)

## Who Maintains And Contributes

This project is maintained/contributed by:

- Robin Richard Canavaggio

Contributions are welcome through pull requests.

Recommended contribution flow:

1. Fork the repository and create a feature branch.
2. Keep changes focused and add/update tests under [tests/inputs](tests/inputs) and [tests/expected](tests/expected) when behavior changes.
3. Run `make test` locally.
4. Open a pull request with a clear summary and rationale.

