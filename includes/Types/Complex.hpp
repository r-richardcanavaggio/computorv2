#pragma once

#include "Types/Real.hpp"

struct Complex
{
	Real real = 0.0;
	Real imag = 0.0;

	Complex() noexcept = default;
	Complex( Real r, Real i ) noexcept;
	Complex( Real r ) noexcept;

	Complex( const Complex& ) noexcept = default;
	Complex( Complex&& ) noexcept = default;
	Complex& operator=( const Complex& ) noexcept = default;
	Complex& operator=( Complex && ) noexcept = default;

	bool isStrictlyReal() const noexcept;

	Complex& operator+=( Complex other ) noexcept;
	Complex& operator-=( Complex other ) noexcept;
	Complex& operator*=( Complex other ) noexcept;
	Complex& operator/=( Complex other );

	friend Complex operator+( Complex a, Complex b ) noexcept { return a += b; }
	friend Complex operator-( Complex a, Complex b ) noexcept { return a -= b; }
	friend Complex operator*( Complex a, Complex b ) noexcept { return a *= b; }
	friend Complex operator/( Complex a, Complex b ) { return a /= b; }

	Complex operator-() const noexcept;

	bool operator==( const Complex & ) const noexcept = default;
};

std::ostream& operator<<( std::ostream &os, Complex z );

namespace maths
{

Complex	conj( Complex z ) noexcept;
Real	abs( Complex z );
Complex	sqrt( Complex z );
Real	norm( Complex z );
Real	norm2( Complex z );
Complex	exp( Complex z ) noexcept;
Complex cos( Complex z ) noexcept;
Complex sin( Complex z ) noexcept;
Complex tan( Complex z );
Complex pow( Complex z, int p );

}