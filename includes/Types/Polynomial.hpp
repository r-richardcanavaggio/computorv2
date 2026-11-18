#pragma once

#include <vector>
#include <string>
#include <variant>
#include <array>

#include "Types/Real.hpp"
#include "Types/Complex.hpp"
#include "Types/Matrix.hpp"

using Root = std::variant<Real, Complex>;

inline std::ostream&	operator<<( std::ostream& os, const Root& r )
{
	std::visit( [&os]( const Root& val ) { os << val; }, r );
	return os;
}

class Polynomial
{
private:
	std::vector<Real>	m_coeffs;
	std::string			m_var = "x";

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

		friend std::ostream& operator<<( std::ostream& os, const Result& res )
		{
			os << "Polynomial degree: " << res.degree << '\n';

			if (res.degree == 0)
			{
				if (res.isInf)
					os << "All real numbers are solution\n";
				else
					os << "No solution\n";
				return os;
			}
			if (res.degree == 1)
				return os << "The solution is:\n" << res.roots.value()[0] << '\n';

			Real	delta = res.delta.value();

			os << "Discriminant is " << delta << '\n';

			if (delta > 0)
				os << "Strictly positive, the two solutions are:\n"
					<< res.roots.value()[0] << '\n' << res.roots.value()[1] << '\n';
			else if (delta == 0)
				os << "Discriminant is null, the solution is:\n"
					<< res.roots.value()[0] << '\n';
			else
				os << "Strictly negative, the two solutions are:\n"
					<< res.roots.value()[0] << '\n' << res.roots.value()[1] << '\n';
			return os;
		}
	};

public:
	Polynomial() = default;

	Polynomial( std::initializer_list<Real>, std::string varName = "x" );
	Polynomial( Real x );

	Polynomial( const Polynomial& ) = default;
	Polynomial( Polynomial&& ) noexcept = default;
	Polynomial&	operator=( const Polynomial& ) = default;
	Polynomial&	operator=( Polynomial&& ) noexcept = default;

	std::size_t	degree() const noexcept;
	const Real&	leadCoef() const noexcept { return m_coeffs.back(); }

	void		setName( std::string_view );

	bool		null() const noexcept
	{
		return m_coeffs.empty() || (m_coeffs.size() == 1 && m_coeffs[0] == 0);
	}

	const Real&	getCoef( int coef ) const { return m_coeffs.at(coef); }

	bool	hasOnlyNullCoeffs() const
	{
		for (std::size_t i = 0; i < m_coeffs.size() - 1; ++i)
			if (m_coeffs[i] != 0)
				return false;
		return true;
	}

	static Polynomial	createSymbolicPolynomial( std::string varName );

	Result		solve() const;

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
	Polynomial&			operator-=( const Polynomial& other );
	Polynomial&			operator*=( const Polynomial& other );
	Polynomial			operator-() const;

	friend Polynomial	operator+( Polynomial a, const Polynomial& b ) { a += b; return a; }
	friend Polynomial	operator-( Polynomial a, const Polynomial& b ) { a -= b; return a; }
	friend Polynomial	operator*( Polynomial a, const Polynomial& b ) { a *= b; return a; }
	friend Polynomial	operator/( Polynomial a, Real scl ) { a *= maths::inv(scl); return a; }
	friend Polynomial	operator%( const Polynomial& a, const Polynomial& b ) { return euclideanDiv(a, b).second; }

	/// @brief Performs euclidean division on A(x) and B(x)
	/// @param const Polynomial& a
	///	@param const Polynomial& b
	/// @return std::pair<Polynomial, Polynomial>, first is quotient Q(x), second is rest R(x)
	friend std::pair<Polynomial, Polynomial>	euclideanDiv( const Polynomial& a, const Polynomial& b )
	{
		if (b.m_coeffs.empty())
			throw std::runtime_error( "Error: division by polynomial zero." );

		Polynomial	remainder(a), quotient, term;

		while (!remainder.m_coeffs.empty() && remainder.degree() >= b.degree())
		{
			auto degDiff = remainder.degree() - b.degree();
			auto coefRatio = remainder.m_coeffs.back() / b.m_coeffs.back();

			term.m_coeffs.resize( degDiff + 1, 0 );
			term.m_coeffs.back() = coefRatio;

			quotient += term;
			remainder -= b * term;

			remainder.trim();
		}
		return { quotient, remainder };
	}
	friend Polynomial	PGCD( Polynomial A, Polynomial B )
	{
		while (!B.null())
		{
			auto	res = euclideanDiv(A, B);
			A = B;
			B = res.second;
		}
		return A / A.m_coeffs.back();
	}

	bool	operator==( const Polynomial& other ) const noexcept = default;

	friend std::ostream& operator<<( std::ostream& os, const Polynomial& p );
};

namespace maths
{

inline Polynomial	pow( const Polynomial& p, int n )
{
	if (n < 0)
		throw std::runtime_error( "Math Error: Negative exponent for polynomials not supported." );

	if (n == 0)
		return Polynomial(1);

	Polynomial	res(p);

	for (int i = 1; i < n; ++i)
		res *= p;
	return res;
}

}