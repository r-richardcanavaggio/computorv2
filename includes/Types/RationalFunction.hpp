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
		if (m_denominator.null())
			throw std::runtime_error("Math Error: Rational function denominator can't be null.");
	}

	RationalFunction( Real numerator, Polynomial denominator ) : m_numerator(numerator), m_denominator(std::move(denominator))
	{
		if (m_denominator.null())
			throw std::runtime_error("Math Error: Rational function denominator can't be null.");
	}

	RationalFunction() = default;

	RationalFunction( const RationalFunction& ) = default;
	RationalFunction( RationalFunction&& ) noexcept = default;
	RationalFunction&	operator=( const RationalFunction& ) = default;
	RationalFunction&	operator=( RationalFunction&& ) noexcept = default;

	RationalFunction&	operator+=( const RationalFunction& other )
	{
		RationalFunction	res;

		res.m_numerator = (m_numerator * other.m_denominator) + (m_denominator * other.m_numerator);
		res.m_denominator = m_denominator * other.m_denominator;
		*this = std::move(res);
		return	*this;
	}
	RationalFunction&	operator-=( const RationalFunction& other )
	{
		RationalFunction	res;

		res.m_numerator = (m_numerator * other.m_denominator) - (m_denominator * other.m_numerator);
		res.m_denominator = m_denominator * other.m_denominator;
		*this = std::move(res);
		return	*this;
	}
	RationalFunction&	operator*=( const RationalFunction& other )
	{
		m_numerator *= other.m_numerator;
		m_denominator *= other.m_denominator;
		return *this;
	}
	RationalFunction&	operator/=( const RationalFunction& other )
	{
		m_numerator *= other.m_denominator;
		m_denominator *= other.m_numerator;
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

	friend std::ostream&	operator<<( std::ostream& os, const RationalFunction& rf )
	{
		return os << rf.m_numerator << "/" << rf.m_denominator;
	}

	Real	eval( Real b ) const
	{
		return m_numerator.eval(b) / m_denominator.eval(b);
	}
	RationalFunction	eval( const Polynomial& p ) const
	{
		return RationalFunction(m_numerator.eval(p), m_denominator.eval(p));
	}
};