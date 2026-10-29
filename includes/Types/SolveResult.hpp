#pragma once

#include <variant>
#include <iostream>
#include <array>
#include "Real.hpp"
#include "Complex.hpp"

using Root = std::variant<Real, Complex>;

struct Solutions
{
	std::array<Root, 2>	data{};
	std::size_t			count = 0;

	constexpr void			push_back( Root r ) { if (count < 2) data[count++] = r; }
	constexpr auto			begin() const noexcept { return data.begin(); }
	constexpr auto			end() const noexcept { return data.begin() + count; }
	constexpr auto			empty() const noexcept { return count == 0; }
	constexpr std::size_t	size() const noexcept { return count; }
	constexpr const Root&	operator[]( std::size_t i ) const { return data[i]; }
};

struct SolveResult
{
	enum class State { Infinite, None, Ok };

	State				state = State::None;
	int					degree = 0;
	Real				delta = 0;
	Solutions			roots{};
};

inline std::ostream&	operator<<( std::ostream& os, const Root& r )
{
	std::visit([&os]( const auto & val ) { os << val; }, r);
	return os;
}

inline std::ostream& operator<<( std::ostream& os, const SolveResult& res )
{
	os << "Polynomial degree: " << res.degree << '\n';
	if (res.degree == 0)
	{
		if (res.state == SolveResult::State::Infinite)
			os << "All real numbers are solution\n";
		else
			os << "No solution\n";
		return os;
	}
	if (res.degree == 1)
	{
		os << "The solution is:\n" << res.roots[0] << '\n';
		return os;
	}
	Real	delta = res.delta;

	os << "Discriminant is " << delta << '\n';
	if (delta > 0)
		os << "Strictly positive, the two solutions are:\n"
			<< res.roots[0] << '\n' << res.roots[1] << '\n';
	else if (delta == 0)
		os << "Discriminant is null, the solution is:\n"
			<< res.roots[0] << '\n';
	else
		os << "Strictly negative, the two solutions are:\n"
			<< res.roots[0] << '\n' << res.roots[1] << '\n';
	return os;
}
