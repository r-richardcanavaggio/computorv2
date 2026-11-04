#pragma once

#include <iostream>

#include "Constants.hpp"
#include "Types/Real.hpp"

class Complex
{
private:
	Real real = 0.0;
	Real imag = 0.0;

public:
	Complex() noexcept = default;
	Complex( Real r, Real i ) noexcept : real(r), imag(i) {}
	Complex( Real r ) noexcept : real(r), imag(0.0) {}

	Complex(const Complex &) noexcept = default;
	Complex(Complex &&) noexcept = default;
	Complex &operator=(const Complex &) noexcept = default;
	Complex &operator=(Complex &&) noexcept = default;

	Complex &operator+=(Complex other) noexcept;
	Complex &operator-=(Complex other) noexcept;
	Complex &operator*=(Complex other) noexcept;
	Complex &operator/=(Complex other);

	friend Complex operator+(Complex a, Complex b) noexcept
	{
		a += b;
		return a;
	}
	friend Complex operator-(Complex a, Complex b) noexcept
	{
		a -= b;
		return a;
	}
	friend Complex operator*(Complex a, Complex b) noexcept
	{
		a *= b;
		return a;
	}
	friend Complex operator/(Complex a, Complex b)
	{
		a /= b;
		return a;
	}

	Complex operator-() const noexcept { return Complex(-real, -imag); }

	bool operator==(const Complex &) const noexcept = default;

	friend Complex conj(Complex z) noexcept { return Complex(z.real, -z.imag); }
	friend Real abs(Complex z) { return sqrt(square(z.real) + square(z.imag)); }

	friend Complex sqrt(Complex z)
	{
		if (z.imag == 0)
		{
			if (z.real >= 0)
				return Complex(sqrt(z.real), 0);
			return Complex(0, sqrt(-z.real));
		}
		Real realPart = sqrt((abs(z) + z.real) / 2.);
		Real sgn = (z.imag > 0) ? Real(1) : Real(-1);
		Real imagPart = sgn * sqrt((abs(z) - z.real) / 2);
		return Complex(realPart, imagPart);
	}

	friend Real norm(Complex z) { return abs(z); }
	friend Real norm2(Complex z) { return square(z.real) + square(z.imag); }

	friend Complex exp(Complex z, std::size_t terms = 50) noexcept
	{
		Complex sum(1.), term(1.);
		for (std::size_t i = 1; i < terms; ++i)
		{
			term *= z / Real(i);
			sum += term;
		}
		return sum;
	}
	friend Complex cos(Complex z, std::size_t terms = 50) noexcept
	{
		Complex iz(-z.imag, z.real), numerator = exp(iz, terms) + exp(-iz, terms);
		return numerator / Real(2);
	}
	friend Complex sin(Complex z, std::size_t terms = 50) noexcept
	{
		Complex iz(-z.imag, z.real), numerator = exp(iz, terms) + exp(-iz, terms);
		return numerator / Complex(0, 2);
	}
	friend Complex tan(Complex z, std::size_t terms = 50)
	{
		return sin(z, terms) / cos(z, terms);
	}
	friend Complex pow(Complex z, int p)
	{
		Real theta = arctan2(z.real, z.imag);
		return Complex(cos(p * theta), sin(p * theta)) * pow(norm(z), p);
	}

	friend std::ostream &operator<<(std::ostream &os, Complex z)
	{
		auto near = [&](Real x, Real target)
		{
			return abs(x - target) < maths::eps;
		};

		Real re = z.real, im = z.imag;

		if (abs(im) < maths::eps)
			return os << re;
		if (abs(re) < maths::eps)
		{
			if (near(im, 1.))
				return os << "i";
			else if (near(im, -1.))
				return os << "-i";
			return os << im << "i";
		}
		os << re << " ";
		if (near(im, 1))
			return os << "+ i";
		else if (near(im, -1))
			return os << " - i";
		else if (im > 0)
			return os << "+ " << im << "i";
		return os << "- " << -im << "i";
	}
};
