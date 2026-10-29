#pragma once

#include <iostream>
#include <exception>
#include <compare>

class Real
{
	private:
		double	m_value = 0.0;

	public:
		constexpr Real() = default;
		constexpr Real( double v ) noexcept : m_value(v) {}

		constexpr Real( const Real& ) = default;
		constexpr ~Real() = default;

		explicit Real( std::string_view );

		constexpr Real	operator+( const Real& other ) const noexcept { return Real(m_value + other.m_value); }
		constexpr Real	operator-( const Real& other ) const noexcept { return Real(m_value - other.m_value); }
		constexpr Real	operator*( const Real& other ) const noexcept { return Real(m_value * other.m_value); }
		constexpr Real	operator/( const Real& other ) const
		{
			if (other.m_value == 0) throw std::runtime_error("Error: cannot divide by zero.");
			return Real(m_value / other.m_value);
		}
		constexpr Real	operator%( const Real& other ) const
		{
			if (other.m_value == 0) throw std::runtime_error("Error: cannot modulo by zero.");
			long long quotient = static_cast<long long>(m_value / other.m_value);

			return Real(m_value - static_cast<double>(quotient) * other.m_value);
		}
		constexpr Real	operator-() const noexcept { return Real(-m_value); }

		constexpr Real&	operator=( const Real& ) = default;
		constexpr Real&	operator+=( const Real& other ) noexcept
		{
			m_value += other.m_value;
			return *this;
		}
		constexpr Real&	operator-=( const Real& other ) noexcept
		{
			m_value -= other.m_value;
			return *this;
		}
		constexpr Real&	operator*=( const Real& other ) noexcept
		{
			m_value *= other.m_value;
			return *this;
		}
		constexpr Real&	operator/=( const Real& other )
		{
			if (other.m_value == 0)	throw std::runtime_error("Error: cannot divide by zero.");
			m_value /= other.m_value;
			return *this;
		}

		constexpr double		getValue() const noexcept { return m_value; }
		constexpr Real			square() const noexcept { return Real(m_value * m_value); }
		constexpr Real			abs() const noexcept { return (m_value < 0) ? -m_value : m_value; }
		constexpr Real			norm() const noexcept { return abs(); }
		constexpr Real			inv() const { if (m_value == 0) throw std::runtime_error("Error: inv() on 0."); return 1 / m_value; }

		constexpr auto			operator<=>( const Real& ) const = default;

		constexpr explicit		operator int() const noexcept { return static_cast<int>(m_value); }
		constexpr explicit		operator double() const noexcept { return m_value; }

		friend std::ostream&	operator<<( std::ostream&, const Real& );
};
