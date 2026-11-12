# computorv2

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue?logo=cplusplus)
![Build](https://img.shields.io/badge/build-Makefile%20%7C%20CMake-green)
![Deps](https://img.shields.io/badge/stdlib_only-zero_external_deps-orange)
![readline](https://img.shields.io/badge/links-readline-lightgrey)
![42 School](https://img.shields.io/badge/school-42-black)
![Lines](https://img.shields.io/badge/LOC-~3000-informational)

A **hand-built mathematical interpreter and Computer Algebra System (CAS)** written in C++20 — no Boost, no parser generator, no symbolic math library. The entire pipeline from lexer to algebraic evaluation is implemented from scratch, including every mathematical primitive.

---

## 📌 Academic / Project Context

**computorv2** is a 42 School project (post-common-core curriculum). The assignment specifies:

- **Zero external dependencies** for math or parsing — the standard library is the ceiling.
- Interactive REPL supporting variable assignment, function definition, and polynomial equation resolution.
- Mandatory support for: real numbers, complex numbers, matrices, and polynomial expressions as first-class types.
- Support for **implicit multiplication** (e.g. `3x`, `2i`), **function composition**, and **symbolic evaluation** (treating undefined variables as formal polynomial indeterminates).
- Equation resolution up to degree 2, with discriminant analysis and complex root output.

The only allowed external link is `readline` (line editing and history in the REPL).

---

## 🏗️ Architecture & Modules

```
┌─────────────────────────────────────────────────────────────────┐
│                            main.cpp                             │
│              readline loop → Interpreter::processLine()         │
└───────────────────────────────┬─────────────────────────────────┘
                                │
              ┌─────────────────▼──────────────────┐
              │             Interpreter            │
              │  Lexer::tokenize()  (regex-based)  │
              │  Command dispatch (list/clear/…)   │
              └────────────────┬───────────────────┘
                               │
              ┌────────────────▼──────────────────┐
              │              Evaluator            │
              │  resolveOperatorArity()           │
              │  insertImplicitMultiplications()  │
              │  LHS/RHS split on '='             │
              │  → executeExpression()            │
              │  → executeAssignment()            │
              │  → executeQuery()   (solve f=rhs) │
              └──────────────┬────────────────────┘
                             │
              ┌──────────────▼─────────────────────┐
              │               Parser               │
              │   Recursive-descent (Pratt-style)  │
              │   parseExpression → parseTerm      │
              │     → parseFactor → parsePower     │
              │       → parsePrimary               │
              │   Produces: NodePtr (AST)          │
              └──────────────┬─────────────────────┘
                             │
              ┌──────────────▼─────────────────────┐
              │            AST Nodes               │
              │  NumberNode / ImaginaryNode        │
              │  VariableNode  (symbolic fallback) │
              │  UnaryOpNode / BinaryOpNode        │
              │  FunctionCallNode                  │
              │  BuiltinFunctionNode               │
              │  MatrixNode                        │
              └──────────────┬─────────────────────┘
                             │  eval(Context&) → VarType
              ┌──────────────▼─────────────────────┐
              │            Type System             │
              │  VarType = variant<                │
              │    Real, Complex,                  │
              │    Polynomial, Matrix<Real>,       │
              │    Matrix<Complex>>                │
              │                                    │
              │  Visitor struct (binary dispatch)  │
              └────────────────────────────────────┘
```

### Component Summary

| Module | File(s) | Responsibility |
|---|---|---|
| **Lexer** | `Interpreter.cpp` | Regex tokenizer; identifies numbers, identifiers, operators, matrix delimiters, `?` query marker |
| **Evaluator** | `Evaluator.cpp`, `EvaluatorNormalize.cpp` | Pre-processing pass (arity resolution, implicit `*` injection), LHS/RHS splitting, assignment / expression / query dispatch |
| **Parser** | `Parser.cpp`, `ParserPrimary.cpp`, `TokenStream.cpp` | Recursive-descent parser; produces a typed AST via `unique_ptr<BaseNode>` |
| **AST Nodes** | `Nodes.cpp`, `Nodes.hpp` | Node hierarchy; each node implements `eval(Context&) → VarType` |
| **Visitor** | `Visitor.hpp`, `Visitor.cpp` | Binary operator dispatch table over all type pairs; implements `std::visit` overload sets |
| **Real** | `Real.hpp`, `Real.cpp` | Value wrapper over `double`; full operator set; Taylor series for `exp`, `sin`, `cos`; Newton's method `sqrt`; continued fraction pretty-printer (π, √n, p/q) |
| **Complex** | `Complex.hpp`, `Complex.cpp` | Algebraic complex arithmetic; Euler-formula trig via `exp(iz)` for complex domain |
| **Polynomial** | `Polynomial.hpp`, `Polynomial.cpp` | Coefficient vector; full ring operators; binary exponentiation; Euclidean division & modulo; degree-0/1/2 solver with discriminant analysis |
| **Matrix\<K\>** | `Matrix.hpp`, `Matrix.tpp` | Generic template over `real_complex` concept; Gauss-Jordan inversion; matrix power (binary exp); Frobenius norm; element-wise and dot-product multiplication |
| **Interpreter** | `Interpreter.hpp`, `Interpreter.cpp`, `Commands.cpp` | REPL driver; session `Context` (variable map + angle mode); command registry via `std::function` map |

---

## 🧠 Engineering Highlights

### 1. Zero-dependency math library — Taylor series and Newton-Raphson from scratch

No `<cmath>` calls are used in the type system. `Real::sqrt` is a Newton-Raphson iterative converger (eps = 1e-9). `maths::exp`, `maths::sin`, `maths::cos` are Taylor series truncated at 50 terms. `maths::arctan` uses a conditionally-inverted Leibniz/Maclaurin series with range reduction. Complex trig is derived from the Euler identity: `cos(z) = (e^(iz) + e^(-iz)) / 2`. This guarantees mathematical consistency across the entire type tower.

### 2. `std::variant` type dispatch with structured binary overloads

`VarType = std::variant<Real, Complex, Polynomial, Matrix<Real>, Matrix<Complex>>` is the uniform value type throughout the system. Binary operations between all type pairs are resolved at runtime via `std::visit(Visitor{op}, lhs, rhs)`. The `Visitor` struct provides exhaustive overloads for valid pairs and throws typed runtime errors for semantically invalid combinations (e.g. `Complex ^ Complex`, `Matrix % Real`). This avoids virtual dispatch while keeping the operation table statically verified by the compiler.

### 3. Pre-parse normalization: arity resolution and implicit multiplication

The raw token stream after lexing is ambiguous: `-` can be unary or binary; `3x` is two tokens with no operator. The `Evaluator::normalize()` pass resolves both in a single linear scan. Arity is determined by tracking whether the parser is in "expects operand" state. Implicit `*` tokens are spliced in wherever an expression-end token is immediately followed by an expression-start token (excluding the `f(` case). This yields a fully unambiguous token stream before the recursive-descent parser is ever invoked.

### 4. Symbolic polynomial evaluation and function composition

`VariableNode::eval()` checks the context map; on a miss, it returns `Polynomial({0, 1})` — a fresh indeterminate `x`. This enables symbolic evaluation: `f(x) = 2x^2 + 3x` works because the RHS is parsed in a context where `x` resolves to the polynomial `x`. Function application (`f(g(x))`) uses `Polynomial::eval(const Polynomial&)`, which substitutes the argument polynomial coefficient-by-coefficient via repeated `pow()` and accumulation. Matrix polynomial evaluation (`Polynomial::eval(const Matrix<K>&)`) computes the matrix polynomial using integer matrix powers — only valid for square matrices.

---

## 🚀 Quick Start

**Requirements:** C++20-compatible compiler (clang++ / g++), GNU `readline`

```bash
# Build
make

# Clean objects
make clean

# Full rebuild
make re

# Remove binary + objects
make fclean
```

The binary is produced at `./computorv2`. Alternatively, using CMake:

```bash
cmake -S . -B build
cmake --build build
```

---

## 💻 Showcase

### Variable assignment, function definition, and equation resolution

```
> f(x) = x^2 - 4
x^2 - 4
> f(x) = 0 ?
Polynomial degree: 2
Discriminant is 16
Strictly positive, the two solutions are:
-2
2
```

### Complex discriminant — imaginary roots

```
> f(x) = x^2 + 1
x^2 + 1
> f(x) = 0 ?
Polynomial degree: 2
Discriminant is -4
Strictly negative, the two solutions are:
-i
i
```

### Matrix algebra — power and inversion

```
> A = [[1,2];[3,4]]
[[1, 2]; [3, 4]]
> A^2
[[7, 10]; [15, 22]]
> inv([[2,1];[5,3]])
[[3, -1]; [-5, 2]]
```

### Polynomial composition and implicit multiplication

```
> f(x) = 3x^2 + 2x
3x^2 + 2x
> g(x) = x + 1
x + 1
> f(x) + g(x) ?
3x^2 + 3x + 1
> f(g(x)) ?
3x^2 + 8x + 5
```
