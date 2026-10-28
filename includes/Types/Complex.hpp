#pragma once

#include <iostream>
#include "Types/Real.hpp"

class Complex
{
	private:
		Real	real = 0.0;
		Real	imag = 0.0;
	
	public:
		constexpr Complex() noexcept = default;
		constexpr Complex( Real r , Real i ) noexcept : real(r), imag(i) {}
		constexpr Complex( Real r ) noexcept : real(r), imag(0.0) {}

		constexpr Complex( const Complex& ) noexcept = default;
		constexpr Complex( Complex&& ) noexcept = default;
		constexpr Complex& operator=( const Complex& ) noexcept = default;
		constexpr Complex& operator=( Complex&& ) noexcept = default;

		constexpr bool		operator==( const Complex& ) const noexcept = default;

		constexpr Real		getReal() const noexcept { return real; }
		constexpr Real		getImag() const noexcept { return imag; }
		constexpr Complex	conj() const noexcept { return Complex(real, -imag); }
		constexpr Complex	operator-() const noexcept { return Complex(-real, -imag); }

		Real				norm() const;
		Complex				pow( int ) const;

		friend constexpr Complex	operator+( Complex a, Complex b ) noexcept
		{
			return Complex(a.real + b.real, a.imag + b.imag);
		}
		constexpr Complex&			operator+=( Complex other ) noexcept
		{
			real += other.real;
			imag += other.imag;
			return *this;
		}

		friend constexpr Complex	operator-( Complex a, Complex b ) noexcept
		{
			return Complex(a.real - b.real, a.imag - b.imag);
		}
		constexpr Complex&			operator-=( Complex other ) noexcept
		{
			real -= other.real;
			imag -= other.imag;
			return *this;
		}

		friend constexpr Complex	operator*( Complex a, Complex b ) noexcept
		{
			return Complex(a.real * b.real - a.imag * b.imag, a.real * b.imag + a.imag * b.real);
		}
		constexpr Complex&			operator*=( Complex other ) noexcept
		{
			Real	oldReal = real;
			Real	oldImag = imag;

			real = oldReal * other.real - oldImag * other.imag;
			imag = oldReal * other.imag + oldImag * other.real;
			return (*this);
		}

		friend constexpr Complex	operator/( Complex a, Complex b )
		{
			Real r = (a.real * b.real + a.imag * b.imag) / (b.real * b.real + b.imag * b.imag);
			Real i = (a.imag * b.real - a.real * b.imag) / (b.real * b.real + b.imag * b.imag);
			return Complex(r, i);
		}
		constexpr Complex&	operator/=( Complex other )
		{
			Real	oldReal = real;
			Real	oldImag = imag;

			real = (oldReal * other.real + oldImag * other.imag) / (other.real * other.real + other.imag * other.imag);
			imag = (oldImag * other.real - oldReal * other.imag) / (other.real * other.real + other.imag * other.imag);
			return *this;
		}
		friend std::ostream&	operator<<( std::ostream&, const Complex& );
};
