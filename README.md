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

## How To Get Started

### Prerequisites

- C++20 compiler (`c++`, `clang++`, or `g++`)
- `make`
- `cmake` (optional alternative build flow)
- `readline` library

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

## Where Users Can Get Help

- Start with REPL command help: run `help` inside computorv2.
- Open a GitHub Issue in this repository for bugs, questions, or feature requests.
- For implementation details, browse:
  - interpreter flow: [srcs/Interpreter](srcs/Interpreter)
  - parser and lexer: [srcs/Parser.cpp](srcs/Parser.cpp), [srcs/Lexer.cpp](srcs/Lexer.cpp)
  - type system: [includes/Types](includes/Types), [srcs/Types](srcs/Types)
