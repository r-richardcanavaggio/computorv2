#include <iostream>

#include "Types/Complex.hpp"
#include "Constants.hpp"

Complex::Complex( Real r, Real i ) noexcept : real(r), imag(i) {}

Complex::Complex( Real r ) noexcept : real(r), imag(0.0) {}

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
    Real denom = maths::square(other.real) + maths::square(other.imag);
    Real oldReal = real;
    Real oldImag = imag;

    imag = (oldImag * other.real - oldReal * other.imag) / denom;
    real = (oldReal * other.real + oldImag * other.imag) / denom;
    return *this;
}

Complex Complex::operator-() const noexcept
{
    return Complex(-real, -imag);
}

namespace maths
{

Complex conj( Complex z ) noexcept
{
    return Complex(z.real, -z.imag);
}

Complex sqrt( Complex z )
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

Real abs( Complex z )
{
    return sqrt(square(z.real) + square(z.imag));
}

Real norm( Complex z )
{
    return abs(z);
}

Real norm2( Complex z )
{
    return square(z.real) + square(z.imag);
}

Complex exp( Complex z ) noexcept
{
    Complex sum(1.), term(1.);
    for (std::size_t i = 1; i < maths::terms; ++i)
    {
        term *= z / Real(i);
        sum += term;
    }
    return sum;
}

Complex cos( Complex z ) noexcept
{
    Complex iz(-z.imag, z.real), numerator = exp(iz) + exp(-iz);
    return numerator / Real(2);
}

Complex sin( Complex z ) noexcept
{
    Complex iz(-z.imag, z.real), numerator = exp(iz) + exp(-iz);
    return numerator / Complex(0, 2);
}

Complex tan( Complex z )
{
    return sin(z) / cos(z);
}

Complex pow( Complex z, int p )
{
    Real theta = arctan2(z.real, z.imag);
    return Complex(cos(theta * p), sin(theta * p)) * pow(norm(z), p);
}

}

std::ostream& operator<<( std::ostream &os, Complex z )
{
    auto near = [&]( Real x, Real target )
    {
        return maths::abs(x - target) < maths::eps;
    };

    Real re = z.real, im = z.imag;

    if (maths::abs(im) < maths::eps)
        return os << re;
    if (maths::abs(re) < maths::eps)
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
