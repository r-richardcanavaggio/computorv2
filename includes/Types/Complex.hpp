#pragma once

#include <iostream>
#include "Types/Real.hpp"

struct Complex
{
	Real	real = 0.0;
	Real	imag = 0.0;
	
	constexpr Complex() noexcept = default;
	constexpr Complex( Real r , Real i ) noexcept : real(r), imag(i) {}
	constexpr Complex( Real r ) noexcept : real(r), imag(0.0) {}

	constexpr Complex( double r ) noexcept : real(r), imag(0.) {}
	constexpr Complex( double r, double i ) noexcept : real(r), imag(i) {}

	constexpr Complex( const Complex& ) noexcept = default;
	constexpr Complex( Complex&& ) noexcept = default;
	constexpr Complex& operator=( const Complex& ) noexcept = default;
	constexpr Complex& operator=( Complex&& ) noexcept = default;

	constexpr Complex& operator+=( Complex other ) noexcept
	{
		real += other.real;
		imag += other.imag;
		return *this;
	}
	constexpr Complex& operator-=( Complex other ) noexcept
	{
		real -= other.real;
		imag -= other.imag;
		return *this;
	}
	constexpr Complex& operator*=( Complex other ) noexcept
	{
		Real	oldReal = real;
		Real	oldImag = imag;

		real = oldReal * other.real - oldImag * other.imag;
		imag = oldReal * other.imag + oldImag * other.real;
		return (*this);
	}
	constexpr Complex& operator/=( Complex other )
	{
		Real	denom = square(other.real) + square(other.imag);
		Real	oldReal = real;
		Real	oldImag = imag;

		imag = (oldImag * other.real - oldReal * other.imag) / denom;
		real = (oldReal * other.real + oldImag * other.imag) / denom;
		return *this;
	}
	friend constexpr Complex operator+( Complex a, Complex b ) noexcept { a += b; return a; }
	friend constexpr Complex operator-( Complex a, Complex b ) noexcept { a -= b; return a; }
	friend constexpr Complex operator*( Complex a, Complex b ) noexcept { a *= b; return a; }
	friend constexpr Complex operator/( Complex a, Complex b ) { a /= b; return a; }
	constexpr Complex operator-() const noexcept { return Complex(-real, -imag); }

	constexpr bool operator==( const Complex& ) const noexcept = default;

	constexpr Complex conj() const noexcept { return Complex(real, -imag); }
	constexpr Real abs() const { return (real * real + imag * imag).sqrt(); }

	constexpr Complex sqrt() const
	{
		if (imag == 0)
		{
			if (real >= 0)
				return Complex(real.sqrt(), 0);
			return Complex(0, (-real).sqrt());
		}
		Real	realPart = ((abs() + real) / 2).sqrt();
		Real	sgn = (imag > 0) ? Real(1) : Real(-1);
		Real	imagPart = sgn * ((abs() - real) / 2).sqrt();
		return Complex(realPart, imagPart);
	}

	friend constexpr Complex conj( Complex z ) noexcept { return z.conj(); }
	friend constexpr Real abs( Complex z ) { return z.abs(); }
	friend constexpr Real norm( Complex z ) { return z.abs(); }
	friend constexpr Real norm2( Complex z ) { return square(z.real) + square(z.imag); }
	friend constexpr Complex sqrt( Complex z ) { return z.sqrt(); }

	friend constexpr Complex exp( Complex z, std::size_t terms = 50 ) noexcept
	{
		Complex	sum(1.), term(1.);
		for (std::size_t i = 1; i < terms; ++i)
		{
			term *= z / Real(i);
			sum += term;
		}
		return sum;
	}
	friend constexpr Complex cos( Complex z, std::size_t terms = 50 ) noexcept
	{
		Complex iz(-z.imag, z.real), numerator = exp(iz, terms) + exp(-iz, terms);
		return numerator / Real(2);
	}
	friend constexpr Complex sin( Complex z, std::size_t terms = 50 ) noexcept
	{
		Complex	iz(-z.imag, z.real), numerator = exp(iz, terms) + exp(-iz, terms);
		return numerator / Complex(0, 2);
	}
	friend constexpr Complex tan( Complex z, std::size_t terms = 50 ) { return sin(z, terms) / cos(z, terms); }
	friend constexpr Complex pow( Complex z, int p )
	{
		Real	theta = arctan2(z.real, z.imag);
		return Complex(cos(p * theta), sin(p * theta)) * pow(norm(z), p);
	}
};

inline std::ostream& operator<<( std::ostream& os, Complex z )
{
	auto near = [&]( Real x, Real target ) { return (abs(x - target) < maths::eps); };

	Real	re = z.real, im = z.imag;

	if (abs(im) < maths::eps) return os << re;
	if (abs(re) < maths::eps)
	{
		if (near(im, 1.)) return os << "i";
		else if (near(im, -1.)) return os << "-i";
		return os << im << "i";
	}
	os << re << " ";
	if (near(im, 1)) return os << "+ i";
	else if (near(im, -1)) return os << " - i";
	else if (im > 0) return os << "+ " << im << "i";
	return  os << "- " << -im << "i";
}
