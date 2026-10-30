#pragma once

#include <iostream>
#include <compare>
#include <charconv>
#include <system_error>
#include <stdexcept>

#include "Constants.hpp"
#include "Fraction.hpp"

struct Real
{
	double	value = 0.0;

	constexpr Real() = default;
	constexpr Real( double v ) noexcept : value(v) {}

	constexpr Real( const Real& ) noexcept = default;
	constexpr Real( Real&& ) noexcept = default;
	constexpr Real& operator=( const Real& ) noexcept = default;
	constexpr Real& operator=( Real&& ) noexcept = default;

	explicit Real( std::string_view s )
	{
		auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);

		if (ec == std::errc::result_out_of_range)
			throw std::runtime_error("Number out of range: " + std::string(s));
		if (ec != std::errc() || ptr != s.data() + s.size())
			throw std::runtime_error("Invalid number format: " + std::string(s));
	}

	constexpr explicit operator double() const noexcept { return value; }
	constexpr explicit operator int() const noexcept { return static_cast<int>(value); }

	constexpr Real& operator+=( Real other ) noexcept { value += other.value; return *this; }
	constexpr Real& operator-=( Real other ) noexcept { value -= other.value; return *this; }
	constexpr Real& operator*=( Real other ) noexcept { value *= other.value; return *this; }
	constexpr Real& operator/=( Real other )
	{
		if (other.value == 0)	throw std::runtime_error("Error: cannot divide by zero.");
		value /= other.value;
		return *this;
	}
	constexpr Real& operator%=( Real other )
	{
		if (other.value == 0) throw std::runtime_error("Error: modulo by zero.");
		long long quotient = static_cast<long long>(value / other.value);
		value -= static_cast<double>(quotient) * other.value;
		return *this;
	}

	friend constexpr Real operator+( Real a, Real b ) noexcept { return a += b; }
	friend constexpr Real operator-( Real a, Real b ) noexcept { a -= b; return a; }
	friend constexpr Real operator*( Real a, Real b ) noexcept { a *= b; return a; }
	friend constexpr Real operator/( Real a, Real b ) { a /= b; return a; }
	friend constexpr Real operator%( Real a, Real b ) { a %= b; return a; }
	constexpr Real operator-() const noexcept { return Real(-value); }
	
	constexpr auto operator<=>( const Real& ) const = default;

	constexpr Real abs() const noexcept { return (value < 0.) ? -value : value; }
	constexpr Real square() const noexcept { return value * value; }
	constexpr Real inv() const
	{
		if (value == 0) throw std::runtime_error("Math Error: inv(0).");
		return 1. / value;
	}

	constexpr Real sqrt() const
	{
		if (value < 0.) throw std::runtime_error("Math Error: Square root of negative number.");
		if (value == 0.) return 0.;
		double	guess = value / 2.;
		while (true)
		{
			double	next = (guess + value / guess) / 2.;
			double	diff = (guess > next) ? (guess - next) : (next - guess);
			if (diff < maths::eps) return next;
			guess = next;
		}
	}

	constexpr Real pow( int p ) const
	{
		if (p == 0) return 1.;
		if (p < 0) return Real(1.) / pow(-p);
		Real	half = pow(p / 2);
		return (p % 2 == 0) ? (half * half) : (half * half * (*this));
	}

	constexpr Real floor() const noexcept
	{
		if (value >= 4503599627370496.0 || value <= -4503599627370496.0) return *this;
		long long i = static_cast<long long>(value);
		if (value < 0. && value != static_cast<double>(i)) return Real(static_cast<double>(i - 1));
		return Real(static_cast<double>(i));
	}
	constexpr Real ceil() const noexcept { return -(-*this).floor(); }
	constexpr Real round() const noexcept { return (value >= 0.) ? (*this + 0.5).floor() : (*this - 0.5).ceil(); }
	constexpr Real trunc() const noexcept { return (value < 0.) ? -(-*this).floor() : floor(); }

	friend constexpr Real abs( Real r ) noexcept { return r.abs(); }
	friend constexpr Real norm( Real r ) noexcept { return r.abs(); }
	friend constexpr Real sqrt( Real r ) { return r.sqrt(); }
	friend constexpr Real square( Real r ) noexcept { return r.square(); }
	friend constexpr Real norm2( Real r ) noexcept { return r.square(); }
	friend constexpr Real inv( Real r ) { return r.inv(); }
	friend constexpr Real pow( Real r, int p ) { return r.pow(p); }
	friend constexpr Real floor( Real r ) { return r.floor(); }
	friend constexpr Real ceil( Real r ) { return r.ceil(); }
	friend constexpr Real round( Real r ) { return r.round(); }
	friend constexpr Real trunc( Real r ) { return r.trunc(); }
	friend constexpr Real min( Real a, Real b ) noexcept { return (b < a) ? b : a; }
	friend constexpr Real max( Real a, Real b ) noexcept { return (a < b) ? b : a; }

	friend constexpr Real exp( Real r, std::size_t terms = 50 ) noexcept
	{
		Real	sum = 1., term = 1.;
		for (std::size_t i = 1; i < terms; ++i)
		{
			term = (term * r) / i;
			sum += term;
		}
		return sum;
	}

	friend constexpr Real cos( Real r, std::size_t terms = 50 ) noexcept
	{
		Real	x = r;
		while (x < 0.) x += 2 * maths::pi;
		while (x > 2. * maths::pi) x -= 2. * maths::pi;
		Real	result = 1., term = 1.;
		for (std::size_t k = 1; k < terms; ++k)
		{
			term *= -x * x / ((2 * k) * (2 * k - 1));
			result += term;
		}
		return result;
	}

	friend constexpr Real sin( Real r, std::size_t terms = 50 ) noexcept
	{
		Real	x = r;
		while (x < -maths::pi) x += 2. * maths::pi;
		while (x > maths::pi) x -= 2. * maths::pi;
		Real	result = x, term = x;
		for (std::size_t k = 1; k < terms; ++k)
		{
			term *= -x * x / ((2 * k) * (2 * k + 1));
			result += term;
		}
		return result;
	}

	friend constexpr Real tan( Real r, std::size_t terms = 50 ) { return sin(r, terms) / cos(r, terms); }

	friend constexpr Real arctan( Real r, std::size_t terms = 50 ) noexcept
	{
		Real	num = (r.abs() <= 1.0) ? r.abs() : (1.0 / r.abs());
		Real	step = num.square();
		Real	sum = 0.;
		int		denom = 1, sign = 1;

		for (std::size_t k = 0; k < terms; ++k)
		{
			sum += Real(sign) * num / denom;
			num *= step;
			denom += 2;
			sign = -sign;
		}
		if (r.abs() > 1.) sum = (maths::pi / 2) - sum;
		if (r.value < 0.) sum = -sum;
		return sum;
	}

	friend constexpr Real arctan2( Real a, Real b, std::size_t terms = 50 ) noexcept
	{
		if (b == 0.) return (a >= 0.) ? 0. : maths::pi;
		if (a == 0.) return (b > 0.) ? maths::pi / 2 : -maths::pi / 2;

		Real	ratio = (b / a).abs();
		Real	alpha = arctan(ratio, terms);

		if (a > 0. && b > 0.) return alpha;
		if (a < 0. && b > 0.) return maths::pi - alpha;
		if (a < 0. && b < 0.) return maths::pi + alpha;
		return -alpha;
	}
};

inline std::ostream&	operator<<( std::ostream& os, Real r )
{
	if (abs(r) < maths::eps) return os << 0;

	if (abs(r - round(r)) < maths::eps) return os << round(r).value;

	if (auto piFrac = Fraction::fromDouble(r.value / maths::pi); piFrac && piFrac->denominator <= 12)
	{
		if (piFrac->numerator == 1) os << "PI";
		else if (piFrac->numerator == -1) os << "-PI";
		else os << piFrac->numerator << "PI";

		if (piFrac->denominator != 1) os << "/" << piFrac->denominator;
		return os;
	}

	Real	rSq = r.square();
	if (abs(rSq - round(rSq) < maths::eps))
	{
		if (round(rSq) > 1. && round(rSq) <= 50)
		{
			if (r < 0.) os << "-";
			return os << "sqrt(" << rSq.value << ")";
		}
	}
	if (auto f = Fraction::fromDouble(r.value); f && f->denominator != 1 && f->denominator <= 100)
		return os << *f;
	return os << r.value;
}
