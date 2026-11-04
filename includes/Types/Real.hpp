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

	friend Real	operator+( Real rhs, Real lhs ) noexcept;
	friend Real	operator-( Real rhs, Real lhs ) noexcept;
	friend Real	operator*( Real rhs, Real lhs ) noexcept;
	friend Real	operator/( Real rhs, Real lhs );
	friend Real	operator%( Real rhs, Real lhs );

	friend Real	abs( Real r );
	friend Real	norm( Real r );
	friend Real	square( Real r ) noexcept;
	friend Real	norm2( Real r ) noexcept;
	friend Real	sqrt( Real r );
	friend Real	inv( Real r );
	friend Real	pow( Real r, int p );
	friend Real floor( Real r );
	friend Real ceil( Real r );
	friend Real round( Real r );
	friend Real min( Real a, Real b ) noexcept;
	friend Real max( Real a, Real b ) noexcept;
	friend Real exp( Real r ) noexcept;
	friend Real cos( Real r ) noexcept;
	friend Real sin( Real r ) noexcept;
	friend Real tan( Real r );
	friend Real arctan( Real r ) noexcept;
	friend Real arctan2( Real a, Real b ) noexcept;

	auto operator<=>( const Real& ) const = default;

	struct Fraction
	{
		long long	numerator = 0;
		long long	denominator = 1;

		Fraction( long long num, long long den = 1 );

		void	simplify();
		static std::optional<Fraction>	createFraction( Real value );
	};
	friend std::ostream&	operator<<( std::ostream& os, Real r );
};
