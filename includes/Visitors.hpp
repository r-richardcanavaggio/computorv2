#pragma once

#include "RuntimeTypes.hpp"

struct BinaryOpVisitor
{
	OpKind	op;

	template <typename T, typename U>
	VarType	operator()( const T& a, const U& b ) const
	{
		switch (op)
		{
			case OpKind::ADD:
				if constexpr (requires { a + b; }) return a + b;
				break;
			case OpKind::SUB:
				if constexpr (requires { a - b; }) return a - b;
				break;
			case OpKind::MUL:
				if constexpr (requires { a * b; }) return a * b;
				break;
			case OpKind::DIV:
				if constexpr (requires { a / b; }) return a / b;
				break;
			case OpKind::MOD: 
				if constexpr (requires { a % b; }) return a % b;
				break;
			case OpKind::POW:
				if constexpr (requires { a.pow(int{}); } && std::is_same_v<U, Real>)
				{
					if (b < 0. || Real(static_cast<int>(b)) != b)
						throw std::runtime_error("Math Error: Exponent must be a positive integer.");
					return a.pow(static_cast<int>(b)); 
				}
				break;
            case OpKind::DOT:
                if constexpr (requires { a.dot(b); })
                    return a.dot(b);
                break;
            case OpKind::NONE:
                break;
		}
		throw std::runtime_error("Error: Invalid operation for given types.");
	}
};

inline VarType	applyUnaryOp( OpKind op, const VarType& a )
{
	auto	unaryVisitor = [op]( const auto& val ) -> VarType
	{
		if (op == OpKind::ADD) return val;
		if (op == OpKind::SUB) return -val;
		throw std::runtime_error("Error: Unknown unary operator.");
	};
	return (std::visit(unaryVisitor, a));
}

inline VarType	applyBinaryOp( OpKind op, const VarType& lhs, const VarType& rhs )
{
	return (std::visit(BinaryOpVisitor{op}, lhs, rhs));
}
