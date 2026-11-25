#!/usr/bin/env python3
"""
End-to-end test suite for ComputorV2.
Usage:
    python3 tests/run_tests.py [path/to/computorv2]
"""

from dataclasses import dataclass, field
from pathlib import Path
import subprocess
import sys
from typing import List, Optional

GREEN = "\033[0;32m"
RED = "\033[0;31m"
YELLOW = "\033[0;33m"
CYAN = "\033[0;36m"
BOLD = "\033[1m"
RESET = "\033[0m"


@dataclass
class TestCase:
    category: str
    name: str
    inputs: List[str]
    expect_out: Optional[List[str]] = None
    contains_out: List[str] = field(default_factory=list)
    expect_err: bool = False
    contains_err: List[str] = field(default_factory=list)


TESTS: List[TestCase] = [
    # =========================================================================
    # 1. Subject PDF Examples & Core Assignments / Queries
    # =========================================================================
    TestCase(
        "Subject & Basics",
        "Variable assignment and case-insensitive lookup",
        ["varA = 2", "varB = 4.242", "varC = -4.3", "vara ?", "varB = ?"],
        expect_out=["2", "4.242", "-4.3", "2", "4.242"],
    ),
    TestCase(
        "Subject & Basics",
        "Variable reassignment across types",
        ["x = 2", "y = x", "y = 7", "y = 2 * i - 4", "x ?", "y ?"],
        expect_out=["2", "2", "7", "-4 + 2i", "2", "-4 + 2i"],
    ),
    TestCase(
        "Subject & Basics",
        "Subject 1D matrix example [[3, 4]] -> [3, 4]",
        ["matA = [[1, 2]; [3, 2]; [3, 4]]", "matB = [[3, 4]]", "matB ?"],
        expect_out=["[[1, 2]; [3, 2]; [3, 4]]", "[3, 4]", "[3, 4]"],
    ),
    TestCase(
        "Subject & Basics",
        "Subject function assignment and evaluation",
        ["funA(x) = 2*x^5 + 4x^2 - 5*x + 4", "funB(y) = 43 * y / (4 % 2 * y)", "funA(2) ?"],
        contains_out=["2x^5 + 4x^2 - 5x + 4", "74"],
    ),
    TestCase(
        "Subject & Basics",
        "Subject polynomial evaluation & equation query",
        ["funA(x) = 2*x^2 - 5*x + 2", "funA(2) ?", "funA(x) = 0 ?"],
        contains_out=["0", "Polynomial degree: 2", "0.5", "2"],
    ),

    # =========================================================================
    # 2. Real Arithmetic, Precedence & Formatting
    # =========================================================================
    TestCase(
        "Real Arithmetic",
        "Operator precedence and parentheses",
        ["2 + 3 * 4", "(2 + 3) * 4", "10 - 4 - 2", "2 ^ 3 ^ 2", "-2 ^ 2", "(-2) ^ 2"],
        expect_out=["14", "20", "4", "512", "-4", "4"],
    ),
    TestCase(
        "Real Arithmetic",
        "Negative powers, modulo, and fractions",
        ["2 ^ -2", "10 % 3", "-10 % 3", "1 / 3", "sqrt(2)"],
        expect_out=["0.25", "1", "-1", "1/3", "sqrt(2)"],
    ),
    TestCase(
        "Real Arithmetic",
        "Implicit multiplication",
        ["a = 3", "2a", "2(a + 1)", "(a + 1)(a - 1)"],
        expect_out=["3", "6", "8", "8"],
    ),
    TestCase(
        "Real Arithmetic",
        "Division by zero error",
        ["5 / 0"],
        expect_err=True,
        contains_err=["divide by zero"],
    ),
    TestCase(
        "Real Arithmetic",
        "Modulo by zero error",
        ["5 % 0"],
        expect_err=True,
        contains_err=["modulo by zero"],
    ),

    # =========================================================================
    # 3. Complex Numbers
    # =========================================================================
    TestCase(
        "Complex Numbers",
        "Basic complex arithmetic and collapsing to Real",
        ["i * i", "2i * 3i", "(1 + 2i) + (3 - 2i)", "(1 + 2i) * (3 - 4i)", "(1 + i) / (1 - i)"],
        expect_out=["-1", "-6", "4", "11 + 2i", "i"],
    ),
    TestCase(
        "Complex Numbers",
        "Complex formatting (unit imaginary + and -)",
        ["2 + i", "2 - i", "-i", "0 + 3i", "ComplexVar = 1 - 2i"],
        expect_out=["2 + i", "2 - i", "-i", "3i", "1 - 2i"],
    ),
    TestCase(
        "Complex Numbers",
        "Complex integer power and division by Real/Complex",
        ["(1 + i) ^ 2", "1 / i"],
        expect_out=["2i", "-i"],
    ),
    TestCase(
        "Complex Numbers",
        "Assigning to 'i' is forbidden",
        ["i = 5"],
        expect_err=True,
    ),

    # =========================================================================
    # 4. Variables & Interpreter Commands
    # =========================================================================
    TestCase(
        "Commands & Context",
        "Built-in function redefinition is forbidden",
        ["cos = 5", "sin(x) = 2*x"],
        expect_err=True,
        contains_err=["Forbidden"],
    ),
    TestCase(
        "Commands & Context",
        "list, history, clear <var>, exit",
        ["myvar = 42", "list", "history", "clear myvar", "myvar ?", "exit", "999"],
        contains_out=[
            "42",
            "myvar: Real = 42",
            "1: myvar = 42 -> 42",
            "Info: Variable 'myvar' has been deleted.",
            "myvar",  # after clear, 'myvar ?' evaluates as symbolic polynomial 'myvar'
            "Exiting ComputorV2, Goodbye!",
        ],
    ),
    TestCase(
        "Commands & Context",
        "clear with missing argument gives usage error",
        ["clear"],
        expect_err=True,
        contains_err=["Usage: clear"],
    ),

    # =========================================================================
    # 5. Matrices & Vectors
    # =========================================================================
    TestCase(
        "Matrices & Vectors",
        "1D vector and 2D matrix arithmetic",
        [
            "u = [1, 2, 3]",
            "v = [[4, 5, 6]]",
            "u + v",
            "u - v",
            "2 * u",
            "u / 2",
        ],
        expect_out=[
            "[1, 2, 3]",
            "[4, 5, 6]",
            "[5, 7, 9]",
            "[-3, -3, -3]",
            "[2, 4, 6]",
            "[0.5, 1, 1.5]",
        ],
    ),
    TestCase(
        "Matrices & Vectors",
        "Vector dot product, Matrix ** vector, vector ** Matrix, outer product",
        [
            "u = [1, 2, 3]",
            "v = [4, 5, 6]",
            "u ** v",
            "[1, 0] ** [0, 1]",
            "M = [[1, 2]; [3, 4]]",
            "w = [5, 6]",
            "M ** w",
            "w ** M",
            "R = [[1, 2, 3]; [4, 5, 6]]",
            "R ** [1, 0, -1]",
            "[[1]; [2]] ** [3, 4]",
        ],
        expect_out=[
            "[1, 2, 3]",
            "[4, 5, 6]",
            "32",
            "0",
            "[[1, 2]; [3, 4]]",
            "[5, 6]",
            "[17, 39]",
            "[23, 34]",
            "[[1, 2, 3]; [4, 5, 6]]",
            "[-2, -2]",
            "[[3, 4]; [6, 8]]",
        ],
    ),
    TestCase(
        "Matrices & Vectors",
        "Matrix power, inverse, norm, and complex vector dot",
        [
            "M = [[1, 1]; [0, 1]]",
            "M ^ 0",
            "M ^ 3",
            "inv(M)",
            "norm([3, 4])",
            "[1 + i, 2 - i] ** [1, i]",
        ],
        expect_out=[
            "[[1, 1]; [0, 1]]",
            "[[1, 0]; [0, 1]]",
            "[[1, 3]; [0, 1]]",
            "[[1, -1]; [0, 1]]",
            "5",
            "2 + 3i",
        ],
    ),
    TestCase(
        "Matrices & Vectors",
        "Singular matrix inverse error",
        ["inv([[1, 2]; [2, 4]])"],
        expect_err=True,
        contains_err=["singular"],
    ),
    TestCase(
        "Matrices & Vectors",
        "Matrix dimension mismatch errors",
        ["[1, 2] + [1, 2, 3]"],
        expect_err=True,
    ),
    TestCase(
        "Matrices & Vectors",
        "Inconsistent row size in matrix literal",
        ["[[1, 2]; [3]]"],
        expect_err=True,
        contains_err=["Inconsistent row size"],
    ),

    # =========================================================================
    # 6. Polynomials
    # =========================================================================
    TestCase(
        "Polynomials",
        "Custom variable identifiers preserved across +, -, *, unary -",
        ["2 * y + 1", "1 - y", "-t^2 + 3*t - 2", "f(z) = 2*z^2 - z + 5"],
        expect_out=["2y + 1", "-y + 1", "-t^2 + 3t - 2", "2z^2 - z + 5"],
    ),
    TestCase(
        "Polynomials",
        "Function parameter shadows existing variable in context",
        ["x = 10", "f(x) = 2*x + 3", "f(1)", "x ?"],
        expect_out=["10", "2x + 3", "5", "10"],
    ),
    TestCase(
        "Polynomials",
        "Multi-variable polynomial rejected",
        ["x + y"],
        expect_err=True,
        contains_err=["Multi-variable"],
    ),
    TestCase(
        "Polynomials",
        "Polynomial composition and matrix evaluation",
        [
            "f(x) = x^2 + 2*x + 1",
            "g(x) = x - 1",
            "f(g(x))",
            "A = [[1, 1]; [0, 1]]",
            "f(A)",
        ],
        expect_out=[
            "x^2 + 2x + 1",
            "x - 1",
            "x^2",
            "[[1, 1]; [0, 1]]",
            "[[4, 4]; [0, 4]]",
        ],
    ),
    TestCase(
        "Polynomials",
        "Polynomial solving: degree 0 (infinite & no solution), degree 1, degree 2 (delta > 0, = 0, < 0)",
        [
            "p0(x) = 2*x + 1",
            "p0(x) = 2*x + 1 ?",
            "p0(x) = 2*x + 5 ?",
            "p1(x) = 3*x - 6",
            "p1(x) = 0 ?",
            "p2pos(x) = x^2 - 5*x + 6",
            "p2pos(x) = 0 ?",
            "p2zero(x) = x^2 + 2*x + 1",
            "p2zero(x) = 0 ?",
            "p2neg(x) = x^2 + 1",
            "p2neg(x) = 0 ?",
        ],
        contains_out=[
            "All real numbers are solution",
            "No solution",
            "The solution is:\n2",
            "Strictly positive, the two solutions are:\n2\n3",
            "Discriminant is null, the solution is:\n-1",
            "Strictly negative, the two solutions are:\n-i\ni",
        ],
    ),
    TestCase(
        "Polynomials",
        "Polynomial solving degree > 2 throws",
        ["p(x) = x^3 - 1", "p(x) = 0 ?"],
        expect_err=True,
        contains_err=["degree > 2"],
    ),

    # =========================================================================
    # 7. Rational Functions
    # =========================================================================
    TestCase(
        "Rational Functions",
        "PGCD simplification and arithmetic",
        [
            "f(x) = (x^2 - 1) / (x - 1)",
            "g(x) = (x^2 - 3*x + 2) / (x^2 - 4*x + 3)",
            "h(x) = 1 / (x - 1) + 1 / (x + 1)",
        ],
        expect_out=[
            "x + 1",
            "(x - 2)/(x - 3)",
            "2x/(x^2 - 1)",
        ],
    ),
    TestCase(
        "Rational Functions",
        "Rational function evaluation on Real, Polynomial, RationalFunction, and Matrix",
        [
            "f(x) = (x + 1) / (x - 1)",
            "f(3)",
            "f(f(x))",
            "A = [[2, 0]; [0, 3]]",
            "f(A)",
        ],
        expect_out=[
            "(x + 1)/(x - 1)",
            "2",
            "x",
            "[[2, 0]; [0, 3]]",
            "[[3, 0]; [0, 2]]",
        ],
    ),
    TestCase(
        "Rational Functions",
        "Rational function singular matrix denominator throws",
        ["f(x) = (x + 1) / (x - 1)", "B = [[1, 0]; [0, 2]]", "f(B)"],
        expect_err=True,
        contains_err=["singular"],
    ),
    TestCase(
        "Rational Functions",
        "Rational function solving: standard poles, complex denom, constant num, zero num",
        [
            "f1(x) = (x^2 - 1) / (x - 2)",
            "f1(x) = 0 ?",
            "f2(x) = (x - 3) / (x^2 + 1)",
            "f2(x) = 0 ?",
            "f3(x) = 5 / (x - 2)",
            "f3(x) = 0 ?",
            "f4(x) = 0 / (x - 2)",
            "f4(x) = 0 ?",
        ],
        contains_out=[
            "Excluded values (denominator = 0): 2",
            "Domain: all real numbers.",
            "No solution",
            "All real numbers except { 2 } are solutions.",
        ],
    ),
    TestCase(
        "Rational Functions",
        "Rational function solving when denominator degree > 2",
        ["f(x) = (x - 1) / (x^3 + 2)", "f(x) = 0 ?"],
        contains_out=[
            "Note: denominator degree > 2, excluded values not computed.",
            "The solution is:\n1",
        ],
    ),
    TestCase(
        "Rational Functions",
        "Rational function with zero polynomial denominator throws on evaluation",
        ["f(x) = (x + 1) / (x - x)", "f(2)"],
        expect_err=True,
        contains_err=["divide by zero"],
    ),

    # =========================================================================
    # 8. Built-in Math & Trigonometry
    # =========================================================================
    TestCase(
        "Builtins & Trig",
        "Radians vs Degrees trigonometry and real builtins",
        [
            "cos(0)",
            "sin(pi / 2)",
            "deg",
            "sin(90)",
            "cos(180)",
            "rad",
            "abs(-7)",
            "exp(0)",
        ],
        expect_out=[
            "1",
            "1",
            "Info: Angle mode switched to: DEGREES",
            "1",
            "-1",
            "Info: Angle mode switched to: RADIANS",
            "7",
            "1",
        ],
    ),
    TestCase(
        "Builtins & Trig",
        "Complex trigonometry identity: sin(0) == 0 and cos(z)^2 + sin(z)^2 == 1",
        [
            "sin(0 + 0i)",
            "z = 1 + i",
            "cos(z) * cos(z) + sin(z) * sin(z)",
        ],
        expect_out=[
            "0",
            "1 + i",
            "1",
        ],
    ),
    TestCase(
        "Builtins & Trig",
        "sqrt of negative Real throws",
        ["sqrt(-4)"],
        expect_err=True,
        contains_err=["Square root of negative"],
    ),

    # =========================================================================
    # 9. Syntax & Adversarial Edge Cases
    # =========================================================================
    TestCase("Adversarial", "Empty & whitespace lines", ["", "   ", "\t", "1 + 1"], expect_out=["2"]),
    TestCase("Adversarial", "Bare '?' error", ["?"], expect_err=True),
    TestCase("Adversarial", "Bare '=' error", ["="], expect_err=True),
    TestCase("Adversarial", "Unclosed parenthesis", ["(1 + 2"], expect_err=True),
    TestCase("Adversarial", "Extra closing parenthesis", ["1 + 2)"], expect_err=True),
    TestCase("Adversarial", "Unclosed matrix", ["[1, 2"], expect_err=True),
    TestCase("Adversarial", "Unknown token '@'", ["2 @ 3"], expect_err=True),
    TestCase("Adversarial", "Malformed number '1..2'", ["1..2"], expect_err=True),
    TestCase("Adversarial", "Unknown function call", ["unknown_fn(3)"], expect_err=True),
    TestCase("Adversarial", "Solving non-function variable 'a = 0 ?'", ["a = 5", "a = 0 ?"], expect_err=True),

    # =========================================================================
    # 10. Evaluator "Try to Break It" Edge Cases
    # =========================================================================
    TestCase(
        "Evaluator Edge Cases",
        "Expression query '2 + 3 = ?' and solving 'f(x) = g(x) ?'",
        [
            "2 + 3 = ?",
            "f(x) = x^2 - 1",
            "g(x) = x + 1",
            "f(x) = g(x) ?",
        ],
        contains_out=[
            "5",
            "Polynomial degree: 2",
            "Strictly positive, the two solutions are:\n-1\n2",
        ],
    ),
    TestCase(
        "Evaluator Edge Cases",
        "Matrix Hadamard (*) vs matrix mult (**) and complex matrix inverse",
        [
            "A = [[1, 2]; [3, 4]]",
            "B = [[2, 0]; [1, 2]]",
            "A * B",
            "A ** B",
            "C = [[1, i]; [0, 1]]",
            "inv(C)",
            "C ** inv(C)",
        ],
        expect_out=[
            "[[1, 2]; [3, 4]]",
            "[[2, 0]; [1, 2]]",
            "[[2, 0]; [3, 8]]",
            "[[4, 4]; [10, 8]]",
            "[[1, i]; [0, 1]]",
            "[[1, -i]; [0, 1]]",
            "[[1, 0]; [0, 1]]",
        ],
    ),
    TestCase(
        "Evaluator Edge Cases",
        "Variable captured at function definition time & function redefinition",
        [
            "a = 3",
            "f(x) = a * x + 1",
            "a = 10",
            "f(2)",
            "f(x) = x^2",
            "f(3)",
        ],
        expect_out=[
            "3",
            "3x + 1",
            "10",
            "7",
            "x^2",
            "9",
        ],
    ),
    TestCase(
        "Evaluator Edge Cases",
        "Evaluating rational function at its pole throws divide by zero",
        ["f(x) = 1 / (x - 2)", "f(2)"],
        expect_err=True,
        contains_err=["divide by zero"],
    ),
    TestCase("Evaluator Edge Cases", "0 ^ 0 == 1 and 0 ^ -1 throws", ["0 ^ 0", "0 ^ -1"], contains_out=["1"], expect_err=True),
    TestCase("Evaluator Edge Cases", "Negative matrix exponent throws", ["M = [[1, 0]; [0, 1]]", "M ^ -1"], expect_err=True),
    TestCase("Evaluator Edge Cases", "Non-integer matrix exponent throws", ["M = [[1, 0]; [0, 1]]", "M ^ 1.5"], expect_err=True),
    TestCase("Evaluator Edge Cases", "Negative polynomial exponent throws", ["x ^ -1"], expect_err=True),
    TestCase("Evaluator Edge Cases", "Empty RHS variable assignment 'a ='", ["a ="], expect_err=True),
    TestCase("Evaluator Edge Cases", "Empty RHS function assignment 'f(x) ='", ["f(x) ="], expect_err=True),
    TestCase("Evaluator Edge Cases", "Chained '=' error 'a = b = 2'", ["a = b = 2"], expect_err=True),
    TestCase("Evaluator Edge Cases", "Invalid LHS '2 = 2'", ["2 = 2"], expect_err=True),
    TestCase("Evaluator Edge Cases", "Number as function parameter 'f(2) = 5'", ["f(2) = 5"], expect_err=True),

    # =========================================================================
    # 11. Unary Operators
    # =========================================================================
    TestCase(
        "Unary Operators",
        "Valid unary + and - on numbers, parentheses, and binary expressions",
        [
            "+2",
            "-2",
            "-(-2)",
            "-(+2)",
            "+(-2)",
            "-(-(-2))",
            "2 + -3",
            "2 - -3",
            "2 * -3",
            "6 / -2",
            "2 ^ -2",
            "2 ^ +3",
            "-2 ^ -2",
            "-(2 + 3)",
        ],
        expect_out=[
            "2",
            "-2",
            "2",
            "-2",
            "-2",
            "-2",
            "-1",
            "5",
            "-6",
            "-3",
            "0.25",
            "8",
            "-0.25",
            "-5",
        ],
    ),
    TestCase(
        "Unary Operators",
        "Valid unary + and - on variables, complex, matrices, and polynomials",
        [
            "a = 5",
            "-a",
            "+a",
            "-(1 + 2i)",
            "-[1, -2]",
            "-(x^2 - 1)",
        ],
        expect_out=[
            "5",
            "-5",
            "5",
            "-1 - 2i",
            "[-1, 2]",
            "-x^2 + 1",
        ],
    ),
    TestCase("Unary Operators", "Chained unary '--2' rejected", ["--2"], expect_err=True),
    TestCase("Unary Operators", "Chained unary '-----2' rejected", ["-----2"], expect_err=True),
    TestCase("Unary Operators", "Chained unary '++2' rejected", ["++2"], expect_err=True),
    TestCase("Unary Operators", "Mixed chained unary '+-2' rejected", ["+-2"], expect_err=True),
    TestCase("Unary Operators", "Mixed chained unary '-+2' rejected", ["-+2"], expect_err=True),
    TestCase("Unary Operators", "Chained unary in exponent '2 ^ --2' rejected", ["2 ^ --2"], expect_err=True),
    TestCase("Unary Operators", "Chained unary after binary op '2 + --3' rejected", ["2 + --3"], expect_err=True),
    TestCase("Unary Operators", "Chained unary after multiplication '2 * ++3' rejected", ["2 * ++3"], expect_err=True),
    TestCase("Unary Operators", "Chained unary inside parentheses '-(--2)' rejected", ["-(--2)"], expect_err=True),
    TestCase("Unary Operators", "Dangling unary '-' rejected", ["-"], expect_err=True),
    TestCase("Unary Operators", "Dangling unary '+' rejected", ["+"], expect_err=True),

    # =========================================================================
    # 12. UserFunction (AST / Deferred Functions)
    # =========================================================================
    TestCase(
        "UserFunction",
        "Correction test: f(z) = z * y with matrix y, query f(z) = ?, and evaluation",
        [
            "x = 2",
            "y = x * [[4, 2]]",
            "f(z) = z * y",
            "f(z) = ?",
            "f(3)",
        ],
        expect_out=[
            "2",
            "[8, 4]",
            "z * [8, 4]",
            "z * [8, 4]",
            "[24, 12]",
        ],
    ),
    TestCase(
        "UserFunction",
        "Captured variables are frozen at definition time",
        [
            "y = [1, 2]",
            "f(z) = z * y",
            "y = [10, 20]",
            "f(z) = ?",
            "f(2)",
        ],
        expect_out=[
            "[1, 2]",
            "z * [1, 2]",
            "[10, 20]",
            "z * [1, 2]",
            "[2, 4]",
        ],
    ),
    TestCase(
        "UserFunction",
        "Function parameter shadows existing variable in context",
        [
            "z = 99",
            "y = [2, 3]",
            "f(z) = z * y",
            "f(2)",
            "f(z) = ?",
            "z ?",
        ],
        expect_out=[
            "99",
            "[2, 3]",
            "z * [2, 3]",
            "[4, 6]",
            "[198, 297]",
            "99",
        ],
    ),
    TestCase(
        "UserFunction",
        "Querying f(a) = ? with defined variable 'a' evaluates instead of printing definition",
        [
            "y = [3, 4]",
            "f(z) = z * y",
            "a = 2",
            "f(a) = ?",
        ],
        expect_out=[
            "[3, 4]",
            "z * [3, 4]",
            "2",
            "[6, 8]",
        ],
    ),
    TestCase(
        "UserFunction",
        "Transcendental / builtin functions stored as UserFunction and respect deg/rad",
        [
            "f(x) = cos(x)",
            "f(0)",
            "f(pi)",
            "g(x) = sin(x) + 1",
            "deg",
            "g(90)",
        ],
        expect_out=[
            "cos(x)",
            "1",
            "-1",
            "sin(x) + 1",
            "Info: Angle mode switched to: DEGREES",
            "2",
        ],
    ),
    TestCase(
        "UserFunction",
        "Complex and 2D matrix UserFunctions",
        [
            "f(x) = x * i + 1",
            "f(2)",
            "M = [[1, 2]; [3, 4]]",
            "g(x) = x * M + M",
            "g(1)",
        ],
        expect_out=[
            "x * i + 1",
            "1 + 2i",
            "[[1, 2]; [3, 4]]",
            "x * [[1, 2]; [3, 4]] + [[1, 2]; [3, 4]]",
            "[[2, 4]; [6, 8]]",
        ],
    ),
    TestCase(
        "UserFunction",
        "Deferred division by zero throws when called: funB(y) = 43 * y / (4 % 2 * y)",
        [
            "funB(y) = 43 * y / (4 % 2 * y)",
            "funB(2)",
        ],
        expect_err=True,
        contains_err=["divide by zero"],
    ),
    TestCase(
        "UserFunction",
        "Redefining a UserFunction as a Polynomial and vice versa",
        [
            "y = [1, 2]",
            "f(x) = x * y",
            "f(2)",
            "f(x) = 2 * x + 1",
            "f(2)",
        ],
        expect_out=[
            "[1, 2]",
            "x * [1, 2]",
            "[2, 4]",
            "2x + 1",
            "5",
        ],
    ),
    TestCase(
        "UserFunction",
        "Solving equation on non-polynomial UserFunction throws cleanly",
        [
            "y = [1, 2]",
            "f(z) = z * y",
            "f(z) = 0 ?",
        ],
        expect_err=True,
    ),
]


def run_case(binary: Path, tc: TestCase) -> Optional[str]:
    payload = "\n".join(tc.inputs) + "\n"
    proc = subprocess.run(
        [str(binary)],
        input=payload,
        capture_output=True,
        text=True,
        timeout=5,
    )

    if proc.returncode != 0:
        return (
            f"Non-zero exit code ({proc.returncode}) — possible crash / ASan / LSan failure:\n"
            f"STDOUT:\n{proc.stdout}\nSTDERR:\n{proc.stderr}"
        )

    out_lines = [line for line in proc.stdout.splitlines()]
    err_text = proc.stderr.strip()

    if tc.expect_err:
        if not err_text:
            return f"Expected an error on stderr, got none.\nSTDOUT:\n{proc.stdout}"
        for sub in tc.contains_err:
            if sub.lower() not in err_text.lower():
                return f"Expected stderr to contain {sub!r}, got:\n{err_text}"
    else:
        if err_text:
            return f"Unexpected stderr:\n{err_text}\nSTDOUT:\n{proc.stdout}"

    if tc.expect_out is not None and out_lines != tc.expect_out:
        return (
            f"Output mismatch:\n"
            f"  Expected: {tc.expect_out}\n"
            f"  Actual:   {out_lines}"
        )

    for sub in tc.contains_out:
        if sub not in proc.stdout:
            return (
                f"Expected stdout to contain:\n  {sub!r}\n"
                f"Actual stdout:\n{proc.stdout}"
            )

    return None


def main() -> int:
    repo_root = Path(__file__).resolve().parent.parent
    binary = (Path(sys.argv[1]) if len(sys.argv) > 1 else repo_root / "build" / "computorv2").resolve()

    if not binary.exists():
        print(f"{RED}Binary not found at {binary}{RESET}")
        return 2

    print(f"{BOLD}Running {len(TESTS)} test suites against {CYAN}{binary}{RESET}\n")

    passed = 0
    failures = []
    current_cat = None

    for tc in TESTS:
        if tc.category != current_cat:
            current_cat = tc.category
            print(f"{BOLD}{CYAN}== {current_cat} =={RESET}")

        err = run_case(binary, tc)
        if err is None:
            passed += 1
            print(f"  {GREEN}[PASS]{RESET} {tc.name}")
        else:
            failures.append((tc, err))
            print(f"  {RED}[FAIL]{RESET} {tc.name}")

    print(f"\n{BOLD}Summary:{RESET} {passed}/{len(TESTS)} passed", end="")
    if failures:
        print(f", {RED}{len(failures)} failed{RESET}\n")
        for tc, err in failures:
            print(f"{YELLOW}--- FAIL: [{tc.category}] {tc.name} ---{RESET}")
            print(f"Inputs: {tc.inputs}")
            print(f"{err}\n")
        return 1

    print(f" {GREEN}(all green!){RESET}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
