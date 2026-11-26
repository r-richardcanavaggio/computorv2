#pragma once

#include <variant>
#include <span>

#include "Types/Real.hpp"
#include "Types/Complex.hpp"
#include "Types/Matrix.hpp"

using Root = std::variant<Real, Complex>;

inline std::ostream&	operator<<( std::ostream& os, const Root& r )
{
	std::visit([&os]<typename K>( K val ) { os << val; }, r);
	return os;
}

class Polynomial
{
private:
	std::vector<Real>	m_coeffs;
	std::string			m_var = "x";

	using size_t = std::size_t;

	void		trim() noexcept
	{
		while (!m_coeffs.empty() && maths::abs(m_coeffs.back()) < maths::eps)
			m_coeffs.pop_back();
	}

public:
	Polynomial() = default;

	Polynomial( std::initializer_list<Real> list, const std::string& varName = "x" )
		: m_coeffs(list), m_var(varName) { trim(); }

	Polynomial( Real x ) : m_coeffs({x}) { trim(); }

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

	int					degree() const
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

	void				mergeVar( const Polynomial& other )
	{
		if (degree() <= 0 && other.degree() > 0)
			m_var = other.m_var;
		else if (degree() > 0 && other.degree() > 0 && m_var != other.m_var)
			throw std::runtime_error("Error: Multi-variable polynomials are not supported.");
	}

	Polynomial			operator-() const
	{
		Polynomial result;

		result.m_coeffs.reserve(m_coeffs.size());
		for (auto val : m_coeffs)
			result.m_coeffs.push_back(-val);
		result.m_var = m_var;
		return result;
	}

	Polynomial&			operator+=( const Polynomial& other )
	{
		mergeVar(other);

		if (m_coeffs.size() < other.m_coeffs.size())
			m_coeffs.resize(other.m_coeffs.size(), 0);

		for (std::size_t i = 0; i < other.m_coeffs.size(); i++)
			m_coeffs[i] += other.m_coeffs[i];
		trim();
		return *this;
	}

	Polynomial&			operator-=( const Polynomial& other )
	{
		mergeVar(other);

		if (m_coeffs.size() < other.m_coeffs.size())
			m_coeffs.resize(other.m_coeffs.size(), 0);

		for (std::size_t i = 0; i < other.m_coeffs.size(); i++)
			m_coeffs[i] -= other.m_coeffs[i];
		trim();
		return *this;
	}

	Polynomial&			operator*=( const Polynomial& other )
	{
		mergeVar(other);

		if (m_coeffs.empty() || other.m_coeffs.empty())
		{
			m_coeffs.clear();
			return *this;
		}
		std::size_t		  newSize = m_coeffs.size() + other.m_coeffs.size() - 1;
		std::vector<Real> res(newSize, 0);

		for (std::size_t i = 0; i < m_coeffs.size(); ++i)
		{
			for (std::size_t j = 0; j < other.m_coeffs.size(); ++j)
				res[i + j] += m_coeffs[i] * other.m_coeffs[j];
		}
		m_coeffs = std::move(res);
		trim();
		return *this;
	}

	Polynomial&			operator/=( const Real scl ) { *this *= maths::inv(scl); return *this; }

	friend Polynomial	operator+( Polynomial a, const Polynomial& b ) { a += b; return a; }
	friend Polynomial	operator-( Polynomial a, const Polynomial& b ) { a -= b; return a; }
	friend Polynomial	operator*( Polynomial a, const Polynomial& b ) { a *= b; return a; }
	friend Polynomial	operator/( Polynomial a, Real scl ) { a /= scl; return a; }
	friend Polynomial	operator%( const Polynomial& a, const Polynomial& b ) { return euclideanDiv(a, b).second; }

	Polynomial	pow( int n ) const
	{
		if (n < 0)
			throw std::runtime_error("Math Error: Negative exponent for polynomials not supported.");

		if (n == 0)
			return Polynomial(1);

		Polynomial	res(*this);

		for (int i = 1; i < n; ++i)
			res *= *this;
		return res;
	}

	/// @brief Performs euclidean division on A(x) and B(x)
	/// @param const Polynomial& a
	///	@param const Polynomial& b
	/// @return std::pair<Polynomial, Polynomial>, first is quotient Q(x), second is rest R(x)
	friend std::pair<Polynomial, Polynomial>	euclideanDiv( const Polynomial& a, const Polynomial& b )
	{
		if (b.isZero())
			throw std::runtime_error("Error: division by polynomial zero.");

		Polynomial	remainder(a), quotient;
		quotient.m_var = a.m_var;
		remainder.trim();

		while (!remainder.isZero() && remainder.degree() >= b.degree())
		{
			int		degDiff = remainder.degree() - b.degree();
			Real	coefRatio = remainder.m_coeffs.back() / b.m_coeffs.back();
			Polynomial	term;

			term.m_var = a.m_var;
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

	struct Result
	{
		int					degree = 0;
		bool				isInf = false;
		std::vector<Root>	roots{};
		Real				delta{};

		bool	hasRealRoots() const
		{
			for (const auto& root : roots)
			{
				if (const auto ptr = std::get_if<Real>(&root))
					return true;
			}
			return false;
		}

		static Result	solveDegree0( std::span<const Real> coeffs ) noexcept
		{
			if (coeffs.empty() || coeffs[0] == 0)
				return { .isInf = true };
			return {};
		}
		static Result	solveDegree1( std::span<const Real> coeffs )
		{
			return { .degree = 1, .roots = { -coeffs[0] / coeffs[1] }};
		}
		static Result		solveDegree2( std::span<const Real> coeffs )
		{
			Real			  a = coeffs[2], b = coeffs[1], c = coeffs[0];
			Real			  twoA = 2. * a;
			Real			  delta = maths::square(b) - 4. * a * c;
			std::vector<Root> res;

			if (delta > 0)
			{
				res.push_back((-b - maths::sqrt(delta)) / twoA);
				res.push_back((-b + maths::sqrt(delta)) / twoA);
			}
			else if (delta == 0)
				res.push_back(-b / twoA);
			else
			{
				Real re = -b / twoA;
				Real im = maths::sqrt(-delta) / twoA;
				res.push_back(Complex(re, -im));
				res.push_back(Complex(re, im));
			}
			return { .degree = 2, .roots = res, .delta = delta };
		}

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
				return os << "The solution is:\n" << res.roots[0] << '\n';

			Real	delta = res.delta;

			os << "Discriminant is " << delta << '\n';

			if (delta > 0)
				os << "Strictly positive, the two solutions are:\n"
					<< res.roots[0] << '\n' << res.roots[1] << '\n';
			else if (delta == 0)
				os << "Discriminant is null, the solution is:\n"
					<< res.roots[0] << '\n';
			else
				os << "Strictly negative, the two solutions are:\n"
					<< res.roots[0] << '\n' << res.roots[1] << '\n';
			return os;
		}
	};

	Result		solve() const
	{
		switch (degree())
		{
			case -1:
			case 0: return Result::solveDegree0(m_coeffs);
			case 1: return Result::solveDegree1(m_coeffs);
			case 2: return Result::solveDegree2(m_coeffs);
			default: 
				throw std::runtime_error("Math Error: Polynomial evaluation for degree > 2 not supported.");
		}
	}

	Real		eval( Real x ) const
	{
		Real result;

		for (std::size_t i = 0; i < m_coeffs.size(); ++i)
			result += m_coeffs[i] * maths::pow(x, i);
		return result;
	}

	Polynomial	eval( const Polynomial& p ) const
	{
		Polynomial result;

		for (std::size_t i = 0; i < m_coeffs.size(); ++i)
			result += m_coeffs[i] * p.pow(i);
		return result;
	}

	template < typename K >
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
	friend std::ostream& operator<<( std::ostream& os, const Polynomial& p )
	{
		if (p.m_coeffs.empty())
			return os << "0";

		bool printedAny = false;

		for (std::size_t i = p.m_coeffs.size(); i-- > 0;)
		{
			Real coeff = p.m_coeffs[i];

			if (coeff == 0)
				continue;

			if (printedAny)
				os << ( coeff >= 0 ? " + " : " - " );
			else if (coeff < 0)
				os << "-";

			Real mag = maths::abs( coeff );

			if (i == 0 || mag != 1)
				os << mag;
			if (i > 0)
			{
				os << p.m_var;
				if (i > 1)
					os << "^" << i;
			}
			printedAny = true;
		}
		if (!printedAny)
			return os << "0";
		return os;
	}

};

namespace maths
{

inline Polynomial	pow( const Polynomial& p, int n ) { return p.pow(n); }

}