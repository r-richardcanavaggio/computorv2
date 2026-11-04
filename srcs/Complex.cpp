#include "Types/Complex.hpp"
#include "Types/Real.hpp"

Complex& Complex::operator+=( Complex other ) noexcept
{
    real += other.real;
    imag += other.imag;
    return *this;
}
Complex& Complex::operator-=( Complex other ) noexcept
{
    real -= other.real;
    imag -= other.imag;
    return *this;
}
Complex& Complex::operator*=( Complex other ) noexcept
{
    Real oldReal = real;
    Real oldImag = imag;

    real = oldReal * other.real - oldImag * other.imag;
    imag = oldReal * other.imag + oldImag * other.real;
    return *this;
}
Complex& Complex::operator/=( Complex other )
{
    Real denom = square(other.real) + square(other.imag);
    Real oldReal = real;
    Real oldImag = imag;

    imag = (oldImag * other.real - oldReal * other.imag) / denom;
    real = (oldReal * other.real + oldImag * other.imag) / denom;
    return *this;
}