#pragma once

#include "Types/Polynomial.hpp"

class RationalFunction
{
private:
	Polynomial	m_numerator;
	Polynomial	m_denominator;

public:
	explicit RationalFunction( Polynomial numerator, Polynomial denominator ) : m_numerator(std::move(numerator)), m_denominator(std::move(denominator))
	{
		if (m_denominator.isZero())
			throw std::runtime_error("Math Error: Rational function denominator can't be null.");
		normalize();
	}

	RationalFunction( Real numerator, Polynomial denominator ) : m_numerator(numerator), m_denominator(std::move(denominator))
	{
		if (m_denominator.isZero())
			throw std::runtime_error("Math Error: Rational function denominator can't be null.");
		normalize();
	}

	/// @brief RationalFunction representation of a Polynomial
	/// @param numerator 
	RationalFunction( Polynomial numerator ) : m_numerator(numerator), m_denominator(1) { normalize(); }

	/// @brief RationalFunction representation of a Real
	/// @param numerator 
	RationalFunction( Real numerator ) : m_numerator(numerator), m_denominator(1)
	{
		if (m_denominator.isZero())
			throw std::runtime_error("Math Error: Rational function denominator can't be null.");
		normalize();
	}

	RationalFunction() : m_numerator(0), m_denominator(1) {}

	RationalFunction( const RationalFunction& ) = default;
	RationalFunction( RationalFunction&& ) noexcept = default;
	RationalFunction&	operator=( const RationalFunction& ) = default;
	RationalFunction&	operator=( RationalFunction&& ) noexcept = default;

	void	normalize()
	{
		if (!m_numerator.isZero())
		{
			Polynomial	G = PGCD(m_numerator, m_denominator);
	
			if (G.degree() >= 0)
			{
				m_numerator = euclideanDiv(m_numerator, G).first;
				m_denominator = euclideanDiv(m_denominator, G).first;
			}
		}
		if (m_denominator.leadCoef() < 0)
		{
			m_numerator = -m_numerator;
			m_denominator = -m_denominator;
		}
		m_numerator /= m_denominator.leadCoef();
		m_denominator /= m_denominator.leadCoef();
	}

	const Polynomial&	getNumerator() const { return m_numerator; }
	const Polynomial&	getDenominator() const { return m_denominator; }

	RationalFunction&	operator+=( const RationalFunction& other )
	{
		RationalFunction	res;

		res.m_numerator = (m_numerator * other.m_denominator) + (m_denominator * other.m_numerator);
		res.m_denominator = m_denominator * other.m_denominator;
		*this = std::move(res);
		normalize();
		return	*this;
	}
	RationalFunction&	operator-=( const RationalFunction& other )
	{
		RationalFunction	res;

		res.m_numerator = (m_numerator * other.m_denominator) - (m_denominator * other.m_numerator);
		res.m_denominator = m_denominator * other.m_denominator;
		*this = std::move(res);
		normalize();
		return	*this;
	}
	RationalFunction&	operator*=( const RationalFunction& other )
	{
		m_numerator *= other.m_numerator;
		m_denominator *= other.m_denominator;
		normalize();
		return *this;
	}
	RationalFunction&	operator/=( const RationalFunction& other )
	{
		if (other.m_numerator.isZero())
			throw std::runtime_error("Math Error: Can't divide by zero.");

		m_numerator *= other.m_denominator;
		m_denominator *= other.m_numerator;
		normalize();
		return *this;
	}
	RationalFunction	operator-() const
	{
		RationalFunction	res = *this;

		res.m_numerator = -res.m_numerator;
		return res;
	}

	friend RationalFunction	operator+( RationalFunction a, const RationalFunction& b ) { a += b; return a; }
	friend RationalFunction	operator-( RationalFunction a, const RationalFunction& b ) { a -= b; return a; }
	friend RationalFunction	operator*( RationalFunction a, const RationalFunction& b ) { a *= b; return a; }
	friend RationalFunction	operator/( RationalFunction a, const RationalFunction& b ) { a /= b; return a; }

	friend RationalFunction pow( const RationalFunction& f, int n )
	{
		if (n < 0)
			throw std::runtime_error("Error: Can't compute negative power on rational function. (for now)");
		if (n == 0)
			return Polynomial(1);

		return RationalFunction(maths::pow(f.m_numerator, n), maths::pow(f.m_denominator, n));
	}

	friend RationalFunction	compose( const Polynomial& p, const RationalFunction& f )
	{
		RationalFunction	res;

		for (int i = 0; i <= p.degree(); ++i)
			res += p.getCoef(i) * pow(f, i);
		return res;
	}

	friend std::ostream&	operator<<( std::ostream& os, const RationalFunction& rf )
	{
		if (rf.m_denominator.degree() == 0 && rf.m_denominator.getCoef(0) == 1)
			return os << rf.m_numerator;

		if (rf.m_numerator.degree() > 0 && !rf.m_numerator.hasOnlyNullCoeffs())
			os << "(" << rf.m_numerator << ")";
		else
			os << rf.m_numerator;

		if (rf.m_denominator.degree() == 0 && rf.m_denominator.getCoef(0) == 1)
			return os;

		os << "/";

		if (rf.m_denominator.degree() > 0 && !rf.m_denominator.hasOnlyNullCoeffs())
			os << "(" << rf.m_denominator << ")";
		else
			os << rf.m_denominator;

		return os;
	}

	Real	eval( Real b ) const
	{
		return m_numerator.eval(b) / m_denominator.eval(b);
	}

	RationalFunction	eval( const Polynomial& p ) const
	{
		return RationalFunction(m_numerator.eval(p), m_denominator.eval(p));
	}

	RationalFunction	eval( const RationalFunction& other ) const
	{
		RationalFunction	num = compose(m_numerator, other);
		RationalFunction	denum = compose(m_denominator, other);

		return	compose(m_numerator, other) / compose(m_denominator, other);
	}

	template < real_complex K >
	Matrix<K>	eval( const Matrix<K>& m ) const
	{
		Matrix<K>	num = m_numerator.eval(m);
		Matrix<K>	den = m_denominator.eval(m);
		auto		invDen = den.inverse();
	
		if (!invDen)
			throw std::runtime_error("Math Error: Denominator matrix is singular.");
		return num.dot(*invDen);
	}

	struct Result;

	Result	solve() const;

	void	type() const noexcept { std::cout << "RationalFunction"; }
};

struct RationalFunction::Result
{
	RationalFunction	reduced;
	Polynomial::Result	numRes;
	Polynomial::Result	denomRes;

	friend std::ostream& operator<<( std::ostream& os, const Result& res )
	{
		std::vector<Root>	excluded;

		os << "Reduced form: " << res.reduced << " = 0\n";

		if (res.reduced.getDenominator().degree() >= 1)
		{
			if (res.reduced.getDenominator().degree() > 2)
				os << "Note: denominator degree > 2, excluded values not computed.\n";
			else if (!res.denomRes.hasRealRoots())
				os << "Domain: all real numbers.\n";
			else
			{
				excluded = res.denomRes.roots;

				os << "Excluded values (denominator = 0): ";
				if (excluded.size() == 1)
					os << excluded[0];
				else
					os << excluded[0] << ", " << excluded[1];
				os << '\n';
			}
		}
		if (res.reduced.getNumerator().isZero())
		{
			if (excluded.empty())
				return os << "All real numbers are solutions\n";
			os << "All real numbers except { ";
			if (excluded.size() == 1)
				os << excluded[0];
			else
				os << excluded[0] << ", " << excluded[1];
			os << " } are solutions.\n";
			return os;
		}
		if (res.reduced.getNumerator().degree() == 0)
			return os << "No solution\n";
		os << "Solving numerator: " << res.reduced.getNumerator() << " = 0\n" << res.numRes;
		return os;
	}
};

inline RationalFunction::Result	RationalFunction::solve() const
{
	auto	denomRes = (m_denominator.degree() <= 2) ? m_denominator.solve() : Polynomial::Result{};
	return { .reduced = *this, .numRes = m_numerator.solve(), .denomRes = denomRes };
}

namespace maths
{

inline RationalFunction	inv( const RationalFunction& f )
{
	return RationalFunction(f.getDenominator(), f.getNumerator());
}

}
