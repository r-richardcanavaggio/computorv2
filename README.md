# computorv2

`computorv2` is an interactive command-line calculator/interpreter written in **C++20**.
It can parse and evaluate mathematical expressions with support for:

- real numbers
- complex numbers
- polynomials
- matrices (real and complex)
- variables and function calls
- a set of built-in math functions

The project is organized as a small language runtime: **lexer → parser (AST) → interpreter/evaluator**.

---

## 1) Project goals

This project behaves like a REPL (Read-Eval-Print Loop):

1. read a line from the user,
2. tokenize it,
3. parse it into an AST,
4. evaluate it inside a runtime context,
5. print the result or an error.

It is designed to manipulate symbolic and numeric math objects in the same environment.

---

## 2) Stack and dependencies

- **Language:** C++20
- **Compiler:** `c++` (via Makefile)
- **Build system:** Make
- **Terminal input/history:** GNU **readline** (`-lreadline`)
- **Lexing tool:** C++ standard library **`<regex>`** (`std::regex`, `std::sregex_iterator`)
- **Runtime polymorphism:** `std::variant` + `std::visit`
- **Ownership model:** RAII + `std::unique_ptr` for AST nodes

---

## 3) High-level architecture

Main components:

- **Lexer** (`srcs/Lexer.cpp`)  
  Converts input text into `Token` objects.

- **Parser** (`srcs/Parser.cpp`)  
  Implements recursive-descent parsing and builds an **AST**.

- **AST nodes** (`srcs/Nodes/...`)  
  Node types like binary operators, unary operators, numbers, variables, function calls, matrices, etc.

- **Visitors** (`srcs/Visitors/...`)  
  Operator/evaluation-related dispatch logic.

- **Interpreter** (`srcs/Interpreter/...`)  
  Handles line processing, command handling, token preprocessing, and AST evaluation.

- **Math types** (`srcs/Types/...`)  
  Implementations for `Real`, `Complex`, `Polynomial`, and matrix-related behavior.

- **Math helpers** (`srcs/Maths/...`)  
  Arithmetic, exponential, and trigonometric utility operations.

- **Plotter** (`srcs/Plotter.cpp`)  
  Runtime plotting-related support.

---

## 4) Lexing technique

The lexer is regex-based. It recognizes:

- numeric literals (`123`, `3.14`)
- identifiers (`x`, `foo`, `var_2`)
- operators (`+ - * / % ^ **`)
- grouping and matrix tokens (`( ) [ ] , ;`)
- query marker (`?`)

After regex extraction, symbols are mapped to internal token categories (`TokenType`) and operation kinds (`OpKind`).
Identifiers are normalized to lowercase, and `i` is treated specially as the imaginary token.

---

## 5) Parsing technique (AST)

The parser is a **recursive-descent parser**.

It builds an explicit **Abstract Syntax Tree (AST)** using heap-allocated nodes wrapped in `std::unique_ptr` (`NodePtr`).

Implemented precedence levels:

1. primary expressions (numbers, variables, function calls, parenthesized expressions, matrices)
2. power (`^`)
3. unary operators (`+`, `-`)
4. multiplicative (`*`, `/`, `%`, matrix multiply `**`)
5. additive (`+`, `-`)

Matrix literals are parsed as nested expression lists with row separators (`;`) and value separators (`,`).

Built-in function names currently recognized by the parser:

- `cos`, `sin`, `tan`, `sqrt`, `abs`, `exp`, `inv`, `norm`

---

## 6) Runtime model and memory management

### Runtime values

Runtime values are stored in:

- `VarType = std::variant<Real, Complex, Polynomial, Matrix<Real>, Matrix<Complex>>`
- `Context = std::map<std::string, VarType>`

So variables live in a key/value environment where each value is a type-safe variant.

### Memory management strategy

- AST nodes are owned by `std::unique_ptr` (single ownership, automatic cleanup).
- Temporary objects rely on automatic storage and RAII.
- No manual `new/delete` management is needed for AST in normal flow.
- Readline input buffers are released with `free(input)` in the REPL loop.

This design minimizes leaks and makes ownership explicit.

---

## 7) Runtime tools available to the user

At runtime, the program provides:

- an **interactive prompt** (`> `)
- command history via readline
- expression evaluation
- variable/context-based computation
- built-in math functions
- matrix and polynomial manipulation
- interpreter command handling (implemented in `InterpreterCommandHandler.cpp`)

The evaluator/command layer is split in dedicated interpreter source files to keep parsing and execution concerns separated.

---

## 8) Build and run

### Build

```bash
make
```

### Run

```bash
./computorv2
```

### Clean

```bash
make clean
make fclean
```

### Rebuild

```bash
make re
```

### Test suite

```bash
make test
# or
./run_tests.sh
```

---

## 9) Folder overview

- `includes/` — public headers (lexer, parser, types, runtime definitions, etc.)
- `srcs/` — implementation files
  - `Interpreter/`
  - `Nodes/`
  - `Types/`
  - `Visitors/`
  - `Maths/`
- `tests/` — test inputs/expected outputs and diffs
- `Makefile` — build rules
- `run_tests.sh` — regression test script

---

## 10) Notes

- The project uses a classical compiler/interpreter pipeline and modern C++ ownership idioms.
- Core parsing is AST-driven, not direct-eval.
- Type behavior is centralized in dedicated numeric/symbolic classes.
