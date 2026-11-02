#pragma once

#include <concepts>
#include <cstddef>
#include <iostream>
#include <optional>
#include <vector>

#include "Complex.hpp"
#include "Real.hpp"

template <typename K>
concept real_complex = std::same_as<K, Real> || std::same_as<K, Complex>;

template <real_complex K>
class Matrix {
 private:
  std::vector<K> m_elements;
  std::size_t m_rows;
  std::size_t m_cols;

  std::optional<std::size_t> findPivotRow(std::size_t col,
                                          std::size_t startRow) {
    Real bestNorm2 = 0;
    std::size_t bestIndex;

    for (std::size_t r = startRow; r < m_rows; ++r) {
      Real n2 = norm2(at(r, col));
      if (n2 > bestNorm2) {
        bestNorm2 = n2;
        bestIndex = r;
      }
    }

    if (bestNorm2 < maths::eps * maths::eps) return (std::nullopt);
    return (bestIndex);
  }
  void swapRows(std::size_t r1, std::size_t r2) {
    if (r1 == r2) return;

    for (std::size_t c = 0; c < m_cols; ++c) std::swap(at(r1, c), at(r2, c));
  }
  void scaleRow(std::size_t row, K x) {
    for (std::size_t c = 0; c < m_cols; ++c) at(row, c) *= x;
  }
  void addRowMultiple(std::size_t dest, std::size_t src, K factor) {
    for (std::size_t c = 0; c < m_cols; ++c) at(dest, c) -= at(src, c) * factor;
  }

 public:
  Matrix(std::size_t rows, std::size_t cols)
      : m_elements(rows * cols, K(0)), m_rows(rows), m_cols(cols) {}
  Matrix(std::size_t n) : Matrix(n, n) {}

  Matrix(const Matrix&) = default;
  Matrix& operator=(const Matrix&) = default;
  Matrix(Matrix&&) noexcept = default;
  Matrix& operator=(Matrix&&) noexcept = default;

  std::size_t getRows() const noexcept { return m_rows; }
  std::size_t getCols() const noexcept { return m_cols; }

  K& at(std::size_t row, std::size_t col) {
    if (row >= m_rows || col >= m_cols)
      throw std::out_of_range("Matrix::at: index out of range.");
    return m_elements[row * m_cols + col];
  }
  const K& at(std::size_t row, std::size_t col) const {
    if (row >= m_rows || col >= m_cols)
      throw std::out_of_range("Matrix::at: index out of range.");
    return m_elements[row * m_cols + col];
  }
  K& operator()(std::size_t row, std::size_t col) { return at(row, col); }
  const K& operator()(std::size_t row, std::size_t col) const {
    return at(row, col);
  }
  K& operator[](std::size_t index) {
    if (index >= m_elements.size()) throw std::out_of_range("Error: index out of bound.");
    return m_elements[index];
  }
  const K& operator[](std::size_t index) const {
    if (index >= m_elements.size()) throw std::out_of_range("Error: index out of bound.");
    return m_elements[index];
  }

  std::optional<Matrix> inverse() const {
    if (m_rows != m_cols)
      throw std::runtime_error("Error: Matrix must be square to be inverted.");
    if (m_rows == 0) return std::nullopt;

    Matrix result = *this;
    Matrix id = result.identity(m_rows);
    std::size_t row = 0, col = 0;
    std::vector<std::size_t> pivotCol;

    while (row < m_rows && col < result.m_cols) {
      auto pivotRow = result.findPivotRow(col, row);

      if (!pivotRow)
        ++col;
      else {
        result.swapRows(row, *pivotRow);
        id.swapRows(row, *pivotRow);

        K pivot = result(row, col);
        result.scaleRow(row, K(1) / pivot);
        id.scaleRow(row, K(1) / pivot);

        pivotCol.push_back(col);

        for (std::size_t i = row + 1; i < result.getRows(); ++i) {
          K scale = result(i, col);
          result.addRowMultiple(i, row, scale);
          id.addRowMultiple(i, row, scale);
        }
        ++col;
        ++row;
      }
    }
    for (std::size_t r = row; r-- > 0;) {
      std::size_t c = pivotCol[r];
      for (std::size_t i = 0; i < r; i++) {
        K factor = result(i, c);
        if (factor == K(0)) continue;
        result.addRowMultiple(i, r, factor);
        id.addRowMultiple(i, r, factor);
      }
    }
    if (pivotCol.size() != result.m_cols) return (std::nullopt);
    return id;
  }

  Matrix dot(const Matrix& other) const {
    if (m_cols != other.m_rows)
      throw std::runtime_error(
          "Error: wrong dimensions for matrix multiplication");
    Matrix result(m_rows, other.m_cols);

    for (std::size_t r = 0; r < m_rows; ++r) {
      for (std::size_t c = 0; c < other.m_cols; ++c) {
        K sum{};
        for (std::size_t k = 0; k < other.m_rows; k++)
          sum += at(r, k) * other(k, c);
        result(r, c) = sum;
      }
    }
    return result;
  }
  Matrix pow(int p) const {
    if (m_rows != m_cols)
      throw std::runtime_error("Math Error: Power only for square matrices.");
    if (p < 0)
      throw std::runtime_error("Error: Negative matrix power not supported.");

    Matrix res = identity(m_rows);
    Matrix base = *this;

    while (p > 0) {
      if (p % 2 == 1) res = res.dot(base);
      base = base.dot(base);
      p /= 2;
    }
    return res;
  }

  static Matrix identity(std::size_t n) {
    Matrix id(n);
    for (std::size_t i = 0; i < n; ++i) id(i, i) = K(1);
    return id;
  }

  Real norm() const noexcept {
    Real sum = 0;
    for (std::size_t i = 0; i < m_elements.size(); ++i) {
      sum += square(abs(m_elements[i]));
    }
    return (sqrt(sum));
  }

  Matrix& operator+=(const Matrix& other) {
    if (m_rows != other.m_rows || m_cols != other.m_cols)
      throw std::runtime_error(
          "Error: Matrices must have the same dimensions for addition.");
    for (std::size_t i = 0; i < m_elements.size(); ++i)
      m_elements[i] += other.m_elements[i];
    return *this;
  }

  Matrix& operator-=(const Matrix& other) {
    if (m_rows != other.m_rows || m_cols != other.m_cols)
      throw std::runtime_error(
          "Error: Matrices must have the same dimensions for addition");
    for (std::size_t i = 0; i < m_elements.size(); ++i)
      m_elements[i] -= other.m_elements[i];
    return *this;
  }

  Matrix operator-() const {
    Matrix m = *this;
    for (std::size_t i = 0; i < m_elements.size(); ++i) m.m_elements[i] = -m.m_elements[i];
    return m;
  }

  Matrix& operator*=(const Matrix& other) {
    if (m_rows != other.m_rows || m_cols != other.m_cols)
      throw std::runtime_error(
          "Error: dimensions must be identical for element-wise "
          "multiplication");

    for (std::size_t i = 0; i < m_elements.size(); ++i)
      m_elements[i] *= other.m_elements[i];
    return *this;
  }

  friend Matrix operator-(Matrix a, const Matrix& b) {
    a -= b;
    return a;
  }
  friend Matrix operator*(Matrix a, const Matrix& b) {
    a *= b;
    return a;
  }
  Matrix& operator*=(K factor) {
    for (std::size_t i = 0; i < m_elements.size(); ++i) m_elements[i] *= factor;
    return *this;
  }
  friend Matrix operator+(Matrix a, const Matrix& b) {
    a += b;
    return a;
  }

  friend Matrix operator*(Matrix m, K factor) {
    m *= factor;
    return m;
  }
  friend Matrix operator*(K factor, Matrix m) {
    m *= factor;
    return m;
  }

  friend std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    if (m.m_elements.empty()) return os << "[]";

    os << "[";
    for (std::size_t r = 0; r < m.m_rows; ++r) {
      os << "[";
      for (std::size_t c = 0; c < m.m_cols; ++c) {
        os << m(r, c);
        if (c + 1 < m.m_cols) os << ", ";
      }
      os << "]";
      if (r + 1 < m.m_rows) os << "; ";
    }
    os << "]";
    return (os);
  }
};
