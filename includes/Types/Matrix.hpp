#pragma once

#include <concepts>
#include <cstddef>
#include <iostream>
#include <optional>
#include <vector>

#include "Types/Real.hpp"
#include "Types/Complex.hpp"

template < typename K >
concept real_complex = std::same_as< K, Real > || std::same_as< K, Complex >;

template < real_complex K >
class Matrix
{
private:
	std::vector<K>	m_elements;
	std::size_t		m_rows;
	std::size_t		m_cols;

	std::optional<std::size_t>	findPivotRow( std::size_t col,  std::size_t startRow );
  	void						swapRows( std::size_t r1, std::size_t r2 );
	void						scaleRow( std::size_t row, K x );
	void						addRowMultiple( std::size_t dest, std::size_t src, K factor );

public:
	Matrix( std::size_t rows, std::size_t cols );
	Matrix( std::size_t n );

	Matrix( const Matrix& ) = default;
	Matrix& operator=( const Matrix& ) = default;
	Matrix( Matrix&& ) noexcept = default;
	Matrix& operator=( Matrix&& ) noexcept = default;

	std::size_t				getRows() const noexcept;
 	std::size_t				getCols() const noexcept;

	K&						at( std::size_t row, std::size_t col );
	const K&				at( std::size_t row, std::size_t col ) const;
  	K&						operator()( std::size_t row, std::size_t col );
	const K&				operator()( std::size_t row, std::size_t col ) const;
	K&						operator[]( std::size_t index );
	const K&				operator[]( std::size_t index ) const;

	std::optional<Matrix>	inverse() const;

	Matrix					dot( const Matrix& other ) const;
	Matrix					pow( int p ) const;
	static Matrix			identity( std::size_t n );
	Real					norm() const noexcept;

	Matrix&					operator+=( const Matrix& other );
	Matrix&					operator-=(const Matrix& other);
	Matrix&					operator*=( const Matrix& other );
	Matrix					operator-() const;
	Matrix&					operator*=( K factor );
	Matrix&					operator/=( K factor );

	friend Matrix			operator+( Matrix a, const Matrix& b )
	{
		a += b;
		return a;
 	}
	friend Matrix			operator-( Matrix a, const Matrix& b )
	{
		a -= b;
		return a;
	}
	friend Matrix			operator*( Matrix a, const Matrix& b )
	{
		a *= b;
		return a;
	}
	friend Matrix			operator*( Matrix m, K factor )
	{
		m *= factor;
		return m;
	}
	friend Matrix			operator*( K factor, Matrix m )
	{
		m *= factor;
		return m;
	}
	friend Matrix			operator/( K factor, Matrix m )
	{
		m /= factor;
		return m;
	}
	friend Matrix			operator/( Matrix m, K factor )
	{
		m /= factor;
		return m;
	}

	friend std::ostream&	operator<<( std::ostream& os, const Matrix& m )
	{
		if (m.m_elements.empty())
			return os << "[]";

		os << "[";
		for (std::size_t r = 0; r < m.m_rows; ++r)
		{
			os << "[";
			for (std::size_t c = 0; c < m.m_cols; ++c)
			{
				os << m(r, c);
				if (c + 1 < m.m_cols)
					os << ", ";
			}
			os << "]";
			if (r + 1 < m.m_rows)
				os << "; ";
		}
		os << "]";
		return os;
	}
};

#include "Matrix.tpp"