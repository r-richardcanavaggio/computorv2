#pragma once

#include <charconv>
#include <compare>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <optional>

struct Real
{
	double value = 0.0;

	Real() noexcept = default;
	Real( double v ) noexcept;

	explicit Real( std::string_view s );

	Real( const Real& ) noexcept = default;
	Real( Real&& ) noexcept = default;
	Real& operator=( const Real& ) noexcept = default;
	Real& operator=( Real&& ) noexcept = default;

	explicit operator double() const noexcept;
	explicit operator int() const noexcept;
	explicit operator long long() const noexcept;

	Real&	operator+=( Real other ) noexcept;
	Real&	operator-=( Real other ) noexcept;
	Real&	operator*=( Real other ) noexcept;
	Real&	operator/=( Real other );
	Real&	operator%=( Real other );
	Real	operator-() const noexcept;

	friend Real	operator+( Real rhs, Real lhs ) noexcept { rhs += lhs; return rhs; }
	friend Real	operator-( Real rhs, Real lhs ) noexcept { rhs -= lhs; return rhs; }
	friend Real	operator*( Real rhs, Real lhs ) noexcept { rhs *= lhs; return rhs; }
	friend Real	operator/( Real rhs, Real lhs ) { rhs /= lhs; return rhs; }
	friend Real	operator%( Real rhs, Real lhs ) { rhs %= lhs; return rhs; }

	auto operator<=>( const Real& ) const = default;

	struct Fraction
	{
		long long	numerator = 0;
		long long	denominator = 1;

		Fraction( long long num, long long den = 1 );

		void	simplify();
		static std::optional<Fraction>	createFraction( Real value );
	};
};

std::ostream&	operator<<( std::ostream& os, Real r );

namespace maths
{

Real	abs( Real r );
Real	norm( Real r );
Real	square( Real r ) noexcept;
Real	norm2( Real r ) noexcept;
Real	sqrt( Real r );
Real	inv( Real r );
Real	pow( Real r, int p );
Real 	floor( Real r );
Real	ceil( Real r );
Real	round( Real r );
Real	min( Real a, Real b ) noexcept;
Real	max( Real a, Real b ) noexcept;
Real	exp( Real r ) noexcept;
Real	cos( Real r ) noexcept;
Real	sin( Real r ) noexcept;
Real	tan( Real r );
Real	arctan( Real r ) noexcept;
Real	arctan2( Real a, Real b ) noexcept;

}