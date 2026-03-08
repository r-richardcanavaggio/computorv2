#include "Complex.hpp"

Complex		Complex::operator*( const Complex& other ) const noexcept
{
	Complex	z(0, 0);

	z.real = this->real * other.real - this->imag * other.imag;
	z.imag = this->real * other.imag + this->imag * other.real;
	return (z);
}

Complex&	Complex::operator*=( const Complex& other ) noexcept
{
	Real	new_real = this->real;
	Real	new_imag = this->imag;

	new_real = real * other.real - imag * other.imag;
	new_imag = real * other.imag + imag * other.real;
	this->real = new_real;
	this->imag = new_imag;
	return (*this);
}
