#pragma once

#include <variant>
#include <optional>

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

	template < typename T >
	using Optional = std::optional<T>;
	using size_t = std::size_t;

	void		trim() noexcept
	{
		while (!m_coeffs.empty() && maths::abs(m_coeffs.back()) < maths::eps)
			m_coeffs.pop_back();
	}

	struct Result
	{
		const std::vector<Real>&	coeffs;

		short						degree = 0;
		bool						isInf = false;
		Optional<std::vector<Root>>	roots;
		Optional<Real>				delta;

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

	Polynomial( std::initializer_list<Real> list, const std::string& varName = "x" )
		: m_coeffs(list), m_var(varName) { trim(); }

	Polynomial( Real x ) : m_coeffs({x}) {}

	static Polynomial	createSymbolicPolynomial( const std::string& varName ) { return Polynomial({0, 1}, varName); }

	Polynomial( const Polynomial& ) = default;
	Polynomial( Polynomial&& ) noexcept = default;
	Polynomial&	operator=( const Polynomial& ) = default;
	Polynomial&	operator=( Polynomial&& ) noexcept = default;

	bool	operator==( const Polynomial& ) const noexcept = default;

	void				type() const noexcept
	{
		std::cout << "Polynomial";
	}

	bool				isZero() const
	{
		return m_coeffs.empty();
	}

	int	degree() const
	{
		if (isZero())
			return -1;

		return m_coeffs.size() - 1;
	}

	const Real&			leadCoef() const noexcept
	{
		return m_coeffs.back();
	}

	void				setName( const std::string& name )
	{
		m_var = name;
	}

	const Real&			getCoef( int coef ) const
	{
		return m_coeffs.at(coef);
	}

	bool				hasOnlyNullCoeffs() const
	{
		for (size_t i = 0; i < m_coeffs.size() - 1; ++i)
			if (m_coeffs[i] != 0)
				return false;
		return true;
	}

	Polynomial			operator-() const;

	Polynomial&			operator+=( const Polynomial& other );
	Polynomial&			operator-=( const Polynomial& other );
	Polynomial&			operator*=( const Polynomial& other );

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
		if (b.isZero())
			throw std::runtime_error("Error: division by polynomial zero.");

		Polynomial	remainder(a), quotient;
		remainder.trim();

		while (!remainder.isZero() && remainder.degree() >= b.degree())
		{
			int		degDiff = remainder.degree() - b.degree();
			Real	coefRatio = remainder.m_coeffs.back() / b.m_coeffs.back();
			Polynomial	term;

			term.m_coeffs.resize(degDiff + 1, 0);
			term.m_coeffs.back() = coefRatio;

			quotient += term;
			remainder -= b * term;
			remainder.trim();
		}
		return { quotient, remainder };
	}

	friend Polynomial	PGCD( Polynomial A, Polynomial B )
	{
		while (!B.isZero())
		{
			auto	res = euclideanDiv(A, B);
			A = B;
			B = res.second;
		}
		return A / A.m_coeffs.back();
	}

	friend std::ostream& operator<<( std::ostream& os, const Polynomial& p );

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
};

namespace maths
{

Polynomial	pow( const Polynomial&, int );

}