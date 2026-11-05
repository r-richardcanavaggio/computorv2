#include <numeric>

#include "Types/Real.hpp"
#include "Constants.hpp"

Real::Real( double v ) noexcept : value(v) {} 

Real::Real( std::string_view s )
{
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);

    if (ec == std::errc::result_out_of_range)
        throw std::runtime_error("Number out of range: " + std::string(s));
    if (ec != std::errc() || ptr != s.data() + s.size())
        throw std::runtime_error("Invalid number format: " + std::string(s));
}

Real::operator double() const noexcept
{
    return value;
}
Real::operator int() const noexcept
{
    return static_cast<int>(value);
}
Real::operator long long() const noexcept
{
    return static_cast<long long>(value);
}

Real&   Real::operator+=( Real other ) noexcept
{
    value += other.value;
    return *this;
}

Real&   Real::operator-=( Real other ) noexcept
{
    value -= other.value;
    return *this;
}

Real&   Real::operator*=( Real other ) noexcept
{
    value *= other.value;
    return *this;
}

Real&   Real::operator/=( Real other )
{
    if (other.value == 0)
        throw std::runtime_error("Error: cannot divide by zero.");
    value /= other.value;
    return *this;
}

Real&   Real::operator%=( Real other )
{
    if (other.value == 0)
        throw std::runtime_error("Error: modulo by zero.");
    long long quotient = static_cast<long long>(value / other.value);
    value -= static_cast<double>(quotient) * other.value;
    return *this;
}

Real    Real::operator-() const noexcept
{
    return Real(-value);
}

Real Real::operator+( Real other ) const noexcept
{
    return value + other.value;
}

Real Real::operator-( Real other ) const noexcept
{
    return value - other.value;
}

Real Real::operator*( Real other ) const noexcept
{
    return value * other.value;
}

Real Real::operator/( Real other ) const
{
    if (other.value == 0.)
        throw std::runtime_error("Error: cannot divide by zero.");
    return value / other.value;
}

Real Real::operator%( Real other ) const
{
    if (other.value == 0.)
        throw std::runtime_error("Error: cannot modulo by zero.");
    return static_cast<long long>(value) % static_cast<long long>(other.value);
}

namespace maths
{

Real    abs( Real r )
{
    return std::abs(r.value);
}

Real    norm( Real r )
{
    return abs(r);
}

Real    square( Real r ) noexcept
{
    return r.value * r.value;
}

Real    norm2( Real r ) noexcept
{
    return square(r);
}

Real    sqrt( Real r )
{
    if (r < 0.)
        throw std::runtime_error("Math Error: Square root of negative number.");
    if (r == 0.)
        return 0.;
    Real guess = r / 2.;

    while (true)
    {
        Real next = (guess + r / guess) / 2.;
        Real diff = (guess > next) ? (guess - next) : (next - guess);
        if (diff < maths::eps)
            return next;
        guess = next;
    }
}

Real    inv( Real r )
{
    if (r == 0.)
        throw std::runtime_error("Math Error: inv(0).");
    return Real(1) / r;
}

Real    pow( Real r, int p )
{ 
    if (p == 0)
        return 1.;
    if (p < 0)
        return inv(pow(r, -p));
    Real half = pow(r, p / 2);
    return (p % 2 == 0) ? square(half) : square(half) * r;
}

Real    floor( Real r )
{ 
    if (r.value >= 4503599627370496.0 || r.value <= -4503599627370496.0)
        return r;
    long long i = static_cast<long long>(r.value);
    if (r.value < 0. && r.value != static_cast<double>(i))
        return Real(static_cast<double>(i - 1));
    return Real(static_cast<double>(i));
}

Real    ceil( Real r )
{
    return -floor(-r);
}

Real    round( Real r )
{
    return (r.value >= 0.) ? floor(r + 0.5) : ceil(r - 0.5);
}

Real    min( Real a, Real b ) noexcept
{
    return (b < a) ? b : a;
}
Real    max( Real a, Real b ) noexcept
{
    return (a < b) ? b : a;
}

Real    exp( Real r ) noexcept
{
    Real sum = 1., term = 1.;
    for (std::size_t i = 1; i < maths::terms; ++i)
    {
        term = (term * r) / i;
        sum += term;
    }
    return sum;
}

Real    cos( Real r ) noexcept
	{
		Real x = r;
		while (x < 0.) x += 2 * maths::pi;
		while (x > 2. * maths::pi) x -= 2. * maths::pi;
		Real result = 1., term = 1.;
		for (std::size_t k = 1; k < maths::terms; ++k)
		{
			term *= -x * x / ((2 * k) * (2 * k - 1));
			result += term;
		}
		return result;
	}

Real    sin( Real r ) noexcept
{
    Real x = r;
    while (x < -maths::pi) x += 2. * maths::pi;
    while (x > maths::pi) x -= 2. * maths::pi;
    Real result = x, term = x;
    for (std::size_t k = 1; k < maths::terms; ++k)
    {
        term *= -x * x / ((2 * k) * (2 * k + 1));
        result += term;
    }
    return result;
}

Real    tan( Real r )
{
    return sin(r) / cos(r);
}

Real    arctan( Real r ) noexcept
{
    Real num = (abs(r) <= 1.0) ? abs(r) : (Real(1) / abs(r));
    Real step = square(num);
    Real sum = 0.;
    int denom = 1, sign = 1;

    for (std::size_t k = 0; k < maths::terms; ++k) {
        sum += Real(sign) * num / denom;
        num *= step;
        denom += 2;
        sign = -sign;
    }
    if (abs(r) > 1.)
        sum = Real((maths::pi / 2)) - sum;
    if (r.value < 0.)
        sum = -sum;
    return sum;
}

Real    arctan2( Real a, Real b ) noexcept
{
    if (b == 0.)
        return (a >= 0.) ? 0. : maths::pi;
    if (a == 0.)
        return (b > 0.) ? maths::pi / 2 : -maths::pi / 2;

    Real alpha = arctan(abs(b / a));

    if (a > 0. && b > 0.) return alpha;
    if (a < 0. && b > 0.) return Real(maths::pi) - alpha;
    if (a < 0. && b < 0.) return Real(maths::pi) + alpha;
    return -alpha;
}

}

Real::Fraction::Fraction( long long num, long long den ) : numerator(num), denominator(den)
{
    if (denominator == 0)
        throw std::invalid_argument("Error: Fraction denominator cannot be zero.");
    simplify();
}

void    Real::Fraction::simplify()
{
    long long	g = std::gcd(std::abs(numerator), std::abs(denominator));

    if (g != 0)
    {
        numerator /= g;
        denominator /= g;
    }
    if (denominator < 0)
    {
        numerator = -numerator;
        denominator = -denominator;
    }
}

std::optional<Real::Fraction> Real::Fraction::createFraction( Real value )
{
    Real	num = value;
    Real	h1 = 1., h2 = 0.;
    Real	k1 = 0., k2 = 1.;

    for (std::size_t i = 0; i < 64; ++i)
    {
        Real	a = maths::floor(num);
        
        Real	h = a * h1 + h2;
        Real	k = a * k1 + k2;

        h2 = h1; h1 = h;
        k2 = k1; k1 = k;

        if (k != 0.)
        {
            Real	approx = h / k;

            if (maths::abs(value - approx) < maths::eps)
                return Fraction(static_cast<long long>(h), static_cast<long long>(k));
        }
        Real	fracPart = num - a;
        if (maths::abs(fracPart) < maths::eps)
            break;
        num = maths::inv(fracPart);
    }
    return std::nullopt;
}

std::ostream&   operator<<( std::ostream& os, const Real::Fraction& f )
{
    return os << f.numerator << "/" << f.denominator;
}

std::ostream&   operator<<( std::ostream& os, Real r )
{
    if (maths::abs(r) < maths::eps)
        return os << 0;

    if (maths::abs(r - maths::round(r)) < maths::eps)
        return os << maths::round(r).value;

    auto	piFrac = Real::Fraction::createFraction(r.value / maths::pi);
    if (piFrac && piFrac->denominator <= 12)
    {
        if (piFrac->numerator == 1)
        os << "PI";
        else if (piFrac->numerator == -1)
        os << "-PI";
        else
        os << piFrac->numerator << "PI";

        if (piFrac->denominator != 1) os << "/" << piFrac->denominator;
        return os;
    }

    Real rSq = maths::square(r);
    if (maths::abs(rSq - maths::round(rSq)) < maths::eps)
    {
        if (maths::round(rSq) > 1. && maths::round(rSq) <= 50.)
        {
            if (r < 0.) os << "-";
            return os << "sqrt(" << rSq.value << ")";
        }
    }
    auto f = Real::Fraction::createFraction(r.value);
    if (f && f->denominator != 1 && f->denominator <= 100)
        return os << *f;
    return os << r.value;
}
