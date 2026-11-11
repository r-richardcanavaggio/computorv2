#pragma once

#include "RuntimeTypes.hpp"

struct Visitor
{
	OpKind	op = OpKind::NONE;

	Visitor() = default;
	explicit Visitor( OpKind _op ) : op(_op) {}

	VarType operator()( Real ) const;
	VarType operator()( Complex ) const;
	VarType operator()( const Polynomial& ) const;

	template < real_complex K >
	VarType operator()( const Matrix<K>& m ) const
	{
		if (op == OpKind::ADD) return m;
		if (op == OpKind::SUB) return -m;
		throw std::runtime_error("Error: Unkown operator.")
	}

	VarType	operator()( Real, Real ) const;
	VarType operator()( Complex, Real ) const;
	VarType	operator()( Real, Complex ) const;
	VarType	operator()( Complex, Complex ) const;

	template < real_complex K >
	VarType operator()( const Matrix<K>& a, const Matrix<K>& b ) const
	{
		switch (op)
		{
			case OpKind::ADD: return a + b;
			case OpKind::SUB: return a - b;
			case OpKind::MUL: return a * b;
			case OpKind::DOT: return a.dot(b);
			case OpKind::NONE:
				throw std::runtime_error("Error: Unknown operator.");
			default:
				throw std::runtime_error("Error: Operation not supported between Matrix and Matrix types.");
		}
	}
	template < real_complex K >
	VarType operator()( Real scl, const Matrix<K>& m ) const
	{
		switch (op)
		{
			case OpKind::MUL: return scl * m;
			case OpKind::NONE:
				throw std::runtime_error("Error: Unknown operator.");
			default:
				throw std::runtime_error("Error: Operation not supported between Real and Matrix types.");
		}
	}
	template < real_complex K >
	VarType operator()( const Matrix<K>& m, Real scl ) const
	{
		switch (op)
		{
			case OpKind::MUL: return m * scl;
			case OpKind::POW:
				if (scl < 0. || Real(static_cast<int>(scl)) != scl)
					throw std::runtime_error("Math Error: Exponent must be a positive integer.");
				return m.pow(static_cast<int>(scl));
			case OpKind::NONE:
				throw std::runtime_error("Error: Unknown operator.");
			default:
				throw std::runtime_error("Error: Operation not supported between Matrix and Real types.");
		}
	}

	VarType operator()( const Polynomial&, const Polynomial& ) const;
	VarType operator()( const Polynomial&, Real ) const;
	VarType operator()( Real, const Polynomial& ) const;

	template < real_complex K >
	VarType toRadians( K x, const Context& ctx )
	{
		if (ctx.isDegreeMode)
			return x * maths::pi / 180.0;
		return x;
	}

	template < typename T, typename U >
	VarType operator()( const T&, const U& ) const
	{
		throw std::runtime_error("Error: Invalid type operation");
	}
};
