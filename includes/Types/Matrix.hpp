#pragma once

#include <vector>
#include <iostream>
#include <concepts>
#include <optional>
#include <cstddef>

#include "Real.hpp"
#include "Complex.hpp"
#include "Maths/Maths.hpp"

template<typename K>
concept real_complex = 
	std::same_as<K, Real> ||
	std::same_as<K, Complex>;

template<real_complex K>
class Matrix
{
	private:
		std::vector<K>	m_elements;
		std::size_t		m_rows;
		std::size_t		m_cols;

		constexpr std::optional<std::size_t>	findPivotRow( std::size_t col, std::size_t startRow )
		{
			Real		bestNorm2 = 0;
			std::size_t	bestIndex;

			for (std::size_t r = startRow; r < m_rows; ++r)
			{
				Real n2 = maths::scalar_norm2(at(r, col));
				if (n2 > bestNorm2)
				{
					bestNorm2 = n2;
					bestIndex = r;
				}
			}

			if (bestNorm2 < maths::eps * maths::eps)
				return (std::nullopt);
			return (bestIndex);
		}
		constexpr void							swapRows( std::size_t r1, std::size_t r2 )
		{
			if (r1 == r2) return ;

			for (std::size_t c = 0; c < m_cols; ++c)
				std::swap(at(r1, c), at(r2, c));
		}
		constexpr void							scaleRow( std::size_t row, K x )
		{
			for (std::size_t c = 0; c < m_cols; ++c)
				at(row, c) *= x;
		}
		constexpr void							addRowMultiple( std::size_t dest, std::size_t src, K factor )
		{
			for (std::size_t c = 0; c < m_cols; ++c)
				at(dest, c) -= at(src, c) * factor;
		}

	public:
		constexpr Matrix( std::size_t rows, std::size_t cols ) : m_elements(rows * cols, K(0)), m_rows(rows), m_cols(cols) {}
		constexpr Matrix( std::size_t n ) : Matrix(n, n) {}

		constexpr Matrix( const Matrix& ) = default;
		constexpr Matrix& operator=( const Matrix& ) = default;

		constexpr Matrix( Matrix&& ) noexcept = default;
		constexpr Matrix& operator=( Matrix&& ) noexcept = default;

		constexpr std::size_t		size() const noexcept { return m_cols * m_rows; }
		constexpr std::size_t		getRows() const noexcept { return m_rows; }
		constexpr std::size_t		getCols() const noexcept { return m_cols; }
		constexpr bool				empty() const noexcept { return m_elements.empty(); }

		static constexpr Matrix		identity( std::size_t n )
		{
			Matrix	id(n);
			for (std::size_t i = 0; i < n; ++i)
				id(i, i) = K(1);
			return id;
		}
		constexpr Real				norm() const noexcept
		{
			Real	sum = 0;
			for (std::size_t r = 0; r < m_rows; ++r)
			{
				for (std::size_t c = 0; c < m_cols; ++c)
				{
					Real	val = maths::abs(at(r, c));
					sum += val.square();
				}
			}
			return (maths::sqrt(sum));
		}

		constexpr K&				at( std::size_t row, std::size_t col )
		{
			if (row >= m_rows || col >= m_cols) throw std::out_of_range("Matrix::at: index out of range.");
			return m_elements[row * m_cols + col];
		}
		constexpr const K&			at( std::size_t row, std::size_t col ) const
		{
			if (row >= m_rows || col >= m_cols) throw std::out_of_range("Matrix::at: index out of range.");
			return m_elements[row * m_cols + col];
		}
		constexpr K&				operator()( std::size_t row, std::size_t col ) { return at(row, col); }
		constexpr const K&			operator()( std::size_t row, std::size_t col ) const { return at(row, col); }
		constexpr K&				operator[]( std::size_t index )
		{
			if (index >= size()) throw std::out_of_range("Error: index out of bound.");
			return m_elements[index];
		}
		constexpr const K&			operator[]( std::size_t index ) const
		{
			if (index >= size()) throw std::out_of_range("Error: index out of bound.");
			return m_elements[index];
		}

		constexpr std::optional<Matrix>	inverse() const
		{
			if (m_rows != m_cols) throw std::runtime_error("Error: Matrix must be square to be inverted.");
			if (m_rows == 0) return std::nullopt;

			Matrix						result = *this;
			Matrix						id = result.identity(m_rows);
			std::size_t					row = 0, col = 0;
			std::vector<std::size_t>	pivotCol;

			while (row < m_rows && col < result.m_cols)
			{
				auto	pivotRow = result.findPivotRow(col, row);

				if (!pivotRow)
					++col;
				else
				{
					result.swapRows(row, *pivotRow);
					id.swapRows(row, *pivotRow);

					K	pivot = result(row, col);
					result.scaleRow(row, K(1) / pivot);
					id.scaleRow(row, K(1) / pivot);

					pivotCol.push_back(col);

					for (std::size_t i = row + 1; i < result.getRows(); ++i)
					{
						K	scale = result(i, col);
						result.addRowMultiple(i, row, scale);
						id.addRowMultiple(i, row, scale);
					}
					++col;
					++row;
				}
			}
			for (std::size_t r = row; r-- > 0;)
			{
				std::size_t	c = pivotCol[r];
				for (std::size_t i = 0; i < r; i++)
				{
					K factor = result(i, c);
					if (factor == K(0))
						continue;
					result.addRowMultiple(i, r, factor);
					id.addRowMultiple(i, r, factor);
				}
			}
			if (pivotCol.size() != result.m_cols)
				return (std::nullopt);
			return (id);
		}
		
		constexpr Matrix			dot( const Matrix& other ) const
		{
			if (m_cols != other.m_rows)
				throw std::runtime_error("Error: wrong dimensions for matrix multiplication");
			Matrix	result(m_rows, other.m_cols);

			for (std::size_t r = 0; r < m_rows; ++r)
			{
				for (std::size_t c = 0; c < other.m_cols; ++c)
				{
					K	sum = 0;
					for (std::size_t k = 0; k < other.m_rows; k++)
						sum += at(r, k) * other(k, c);
					result(r, c) = sum;
				}
			}
			return result;
		}
		constexpr Matrix			pow( int p ) const
		{
			if (m_rows != m_cols)
				throw std::runtime_error("Math Error: Power only for square matrices.");
			if (p < 0)
				throw std::runtime_error("Error: Negative matrix power not supported.");

			Matrix	res = identity(m_rows);
			Matrix	base = *this;

			while (p > 0)
			{
				if (p % 2 == 1)
					res = res.dot(base);
				base = base.dot(base);
				p /= 2;
			}
			return res;
		}

		constexpr Matrix&			operator+=( const Matrix& other )
		{
			if (m_rows != other.m_rows || m_cols != other.m_cols)
				throw std::runtime_error("Error: Matrices must have the same dimensions for addition.");
			for (std::size_t i = 0; i < size(); ++i)
				m_elements[i] += other.m_elements[i];
			return *this;
		}
		friend constexpr Matrix		operator+( Matrix a, const Matrix& b ) { a += b; return a; }

		constexpr Matrix&			operator-=( const Matrix& other )
		{
			if (m_rows != other.m_rows || m_cols != other.m_cols)
				throw std::runtime_error("Error: Matrices must have the same dimensions for addition");
			for (std::size_t i = 0; i < size(); ++i)
				m_elements[i] -= other.m_elements[i];
			return *this;
		}
		friend constexpr Matrix		operator-( Matrix a, const Matrix& b ) { a -= b; return a; }
		constexpr Matrix			operator-() const
		{
			Matrix	m = *this;
			for (std::size_t i = 0; i < size(); ++i)
				m.m_elements[i] = -m.m_elements[i];
			return m;
		}

		constexpr Matrix&			operator*=( const Matrix& other )
		{
			if (m_rows != other.m_rows || m_cols != other.m_cols)
				throw std::runtime_error("Error: dimensions must be identical for element-wise multiplication");

			for (std::size_t i = 0; i < size(); ++i)
				m_elements[i] *= other.m_elements[i];
			return *this;
		}
		friend constexpr Matrix		operator*( Matrix a, const Matrix& b ) { a *= b; return a; }

		constexpr Matrix&			operator*=( K factor )
		{
			for (std::size_t i = 0; i < size(); ++i)
				m_elements[i] *= factor;
			return *this;
		}
		friend constexpr Matrix		operator*( Matrix m, K factor ) { m *= factor; return m; }
		friend constexpr Matrix		operator*( K factor, Matrix m ) { m *= factor; return m; }

		template<real_complex T>
		friend std::ostream&		operator<<( std::ostream&, const Matrix<T>& );
};

template<real_complex K>
inline std::ostream&	operator<<( std::ostream& os, const Matrix<K>& m )
{
    if (m.empty()) return os << "[]";

    os << "[";
	for (std::size_t r = 0; r < m.getRows(); ++r)
	{
        os << "[";
        for (std::size_t c = 0; c < m.getCols(); ++c)
        {
            os << m(r, c);
            if (c + 1 < m.getCols())
                os << ", ";
        }
        os << "]";
		if (r + 1 < m.getRows())
            os << "; ";
	}
    os << "]";
	return (os);
}
