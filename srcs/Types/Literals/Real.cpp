#include "Real.hpp"
#include "Maths/Maths.hpp"
#include "Fraction.hpp"
#include <charconv>
#include <system_error>

Real::Real( std::string_view s )
{
	auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), m_value);

	if (ec == std::errc::result_out_of_range)
		throw std::runtime_error("Number out of range: " + std::string(s));
	if (ec != std::errc() || ptr != s.data() + s.size())
		throw std::runtime_error("Invalid number format: " + std::string(s));
}

std::ostream&	operator<<( std::ostream& os, const Real& r )
{
	double			val = r.getValue();

	if (maths::abs(val) < maths::eps)
	{
		os << 0;
		return (os);
	}

	if (maths::abs(val - maths::round(val)) < maths::eps)
	{
		os << maths::round(val);
		return (os);
	}

	Fraction	piFrac(0, 1);

	if (Fraction::fromDouble(val / maths::pi, piFrac) && piFrac.getDenom() <= 12)
	{
		if (piFrac.getNum() == 1)
			os << "PI";
		else if (piFrac.getNum() == -1)
			os << "-PI";
		else
			os << piFrac.getNum() << "PI";

		if (piFrac.getDenom() != 1)
			os << "/" << piFrac.getDenom();
		return (os);
	}

	double	squared = val * val;

	if (maths::abs(squared - maths::round(squared)) < maths::eps)
	{
		long long intSq = maths::round(squared);

		if (intSq > 1 && intSq <= 50)
		{
			if (val < 0.0)
				os << "-";
			os << "sqrt(" << intSq << ")";
			return (os);
		}
	}

	Fraction	f(0, 1);

	if (Fraction::fromDouble(val, f) && f.getDenom() != 1 && f.getDenom() <= 100)
		os << f;
	else
		os << val;
	return (os);
}