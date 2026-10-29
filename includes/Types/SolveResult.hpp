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

std::ostream&   operator<<( std::ostream& os, const SolveResult& res );
std::ostream&	operator<<( std::ostream& os, const Root& r );
