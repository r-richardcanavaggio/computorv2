#pragma once

#include <vector>
#include <iostream>
#include <concepts>
#include <optional>
#include <cstddef>

#include "Real.hpp"
#include "Complex.hpp"

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

		K&							at( std::size_t, std::size_t );
		const K&					at( std::size_t, std::size_t ) const;
		void						scl( const Real& );
		void						add( const Matrix& );
		void						sub( const Matrix& );

		std::optional<std::size_t>	findPivotRow( std::size_t, std::size_t );
		void						swap_rows( std::size_t, std::size_t );
		void						scale_row( std::size_t, K );
		void						add_row_multiple( std::size_t, std::size_t, K );

	public:
		Matrix( std::size_t, std::size_t );
		Matrix( std::size_t );
		Matrix( const std::vector<std::vector<K>>& );
		Matrix( const Matrix& ) = default;
		Matrix& operator=( const Matrix& ) = default;	
		~Matrix() = default;
		
		std::size_t					getSize() const;
		std::size_t					getRows() const;
		std::size_t					getCols() const;
		bool						empty() const;
		Matrix<K>					identity() const;
		Real						norm() const;

		std::optional<Matrix<K>>	inverse() const;
		
		Matrix<K>					dot( const Matrix<K>& ) const;
		Matrix<K>					pow( int ) const;
		Matrix						operator+( const Matrix& ) const;
		Matrix						operator-( const Matrix& ) const;
		Matrix						operator-() const;
		Matrix						operator*( const Matrix& ) const;
		Matrix						operator*( const Real& ) const;

		K&							operator()( std::size_t, std::size_t );
		const K&					operator()( std::size_t, std::size_t ) const;
		K&							operator[]( std::size_t );
		const K&					operator[]( std::size_t ) const;
		template<real_complex T>
		friend std::ostream&		operator<<( std::ostream&, const Matrix<T>& );
};
