#pragma once

#include <vector>
#include <string>
#include <variant>
#include <array>

#include "Types/Real.hpp"
#include "Types/Complex.hpp"
#include "Types/Matrix.hpp"

using Root = std::variant<Real, Complex>;

class Polynomial
{
private:
	std::vector<Real>	m_coeffs;
	std::string			m_var = "x";

	Polynomial	euclideanDiv( const Polynomial& b, bool mod = false ) const;
	void		trim() noexcept;

	struct Result
	{
		const std::vector<Real>&			coeffs;

		short								degree = 0;
		bool								isInf = false;
		std::optional<std::vector<Root>>	roots;
		std::optional<Real>					delta;

		Result( const std::vector<Real>& m_coeffs ) : coeffs(m_coeffs) {}
		
		Result&		solveDegree0() noexcept;
		Result&		solveDegree1();
		Result&		solveDegree2();
	};

public:
	Polynomial() = default;
	Polynomial( std::initializer_list<Real> list );
	Polynomial( Real x );

	Polynomial( const Polynomial& ) = default;
	Polynomial( Polynomial&& ) noexcept = default;
	Polynomial&	operator=( const Polynomial& ) = default;
	Polynomial&	operator=( Polynomial&& ) noexcept = default;

	std::size_t	degree() const noexcept;
	void		setName( const std::string& name );

	Result		solve() const;
	Polynomial	pow( int exp ) const;

	Real		eval( Real x ) const;
	Polynomial	eval( const Polynomial& x ) const;

	template < real_complex K >
	Matrix<K>	eval( const Matrix<K>& m ) const
	{
		if (m.getRows() != m.getCols())
			throw std::runtime_error("Error: Evaluation only possible on square matrices");

		Matrix<K>	result(m.getRows(), m.getCols());
		Matrix<K>	id = Matrix<K>::identity(m.getRows());

		for (std::size_t i = 0; i < m_coeffs.size(); ++i)
		{
			if (m_coeffs[i] != 0)
			{
				if (i == 0)
					result += id * m_coeffs[i];
				else
					result += m.pow(i) * m_coeffs[i];
			}
		}
		return result;
	}

	Polynomial&			operator+=( const Polynomial& other );
	Polynomial&			operator+=( Real x );
	Polynomial&			operator-=( const Polynomial& other );
	Polynomial&			operator-=( Real x );
	Polynomial			operator-() const;
	Polynomial&			operator*=( const Polynomial& other );
	Polynomial&			operator*=( Real x );
	Polynomial&			operator/=( const Polynomial& other );
	Polynomial&			operator/=( Real x );
	Polynomial&			operator%=( const Polynomial& other );

	friend Polynomial	operator+( Polynomial a, const Polynomial& b ) { a += b; return a; }
	friend Polynomial	operator+( Polynomial p, Real x ) { p += x; return p; }
	friend Polynomial	operator-( Polynomial a, const Polynomial& b ) { a -= b; return a; }
	friend Polynomial	operator-( Polynomial p, Real x ) { p -= x; return p; }
	friend Polynomial	operator*( Polynomial a, const Polynomial& b ) { a *= b; return a; }
	friend Polynomial	operator*( Polynomial p, Real x ) { p *= x; return p; }
	friend Polynomial	operator/( Polynomial a, const Polynomial& b ) { a /= b; return a; }
	friend Polynomial	operator/( Polynomial p, Real x ) { p /= x; return p; }
	friend Polynomial	operator%( Polynomial a, const Polynomial& b ) { a %= b; return a; }

	bool	operator==( const Polynomial& other ) const noexcept = default;

	friend std::ostream& operator<<( std::ostream& os, const Polynomial& p );
	friend std::ostream& operator<<( std::ostream& os, const Polynomial::Result& roots );
};
