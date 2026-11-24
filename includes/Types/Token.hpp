#pragma once 

#include <string>
#include <optional>
#include <iostream>

struct Token
{
	enum class Type
	{
		NUMBER,
		VARIABLE,
		IMAGINARY,
		OPERATOR,
		EQUAL,
		BRACKET_OPEN,
		BRACKET_MATRIX_OPEN,
		BRACKET_CLOSE,
		BRACKET_MATRIX_CLOSE,
		COMMA,
		SEMICOLON,
		QUERY,
		UNKOWN
	};

	enum class Arity
	{
		UNARY,
		BINARY,
		CONSTANT
	};

	enum class Operator
	{
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,
		POW,
		DOT,
		NONE
	};

	Type		type = Type::UNKOWN;
	Operator	op = Operator::NONE;
	std::string	value{};
	Arity		arity = Arity::CONSTANT;
	
	bool	operator==( const Token& other ) const = default;

	std::string_view toString() const noexcept
	{
		switch (type)
		{
			case Type::IMAGINARY:            return "i";
			case Type::EQUAL:                return "=";
			case Type::BRACKET_OPEN:         return "(";
			case Type::BRACKET_CLOSE:        return ")";
			case Type::BRACKET_MATRIX_OPEN:  return "[";
			case Type::BRACKET_MATRIX_CLOSE: return "]";
			case Type::COMMA:                return ",";
			case Type::SEMICOLON:            return ";";
			case Type::QUERY:                return "?";
			case Type::OPERATOR:
				switch (op)
				{
					case Operator::ADD: return "+";
					case Operator::SUB: return "-";
					case Operator::MUL: return "*";
					case Operator::DIV: return "/";
					case Operator::MOD: return "%";
					case Operator::POW: return "^";
					case Operator::DOT: return "**";
					default:            return "?";
				}
			default:
				return value;
		}
	}
	
	friend std::ostream& operator<<( std::ostream& os, const Token& token )
	{
		switch (token.type)
		{
			case Token::Type::NUMBER: os << "Num"; break;
			case Token::Type::VARIABLE: os << "Var"; break;
			case Token::Type::IMAGINARY: os << "Imaginary"; break;
			case Token::Type::OPERATOR: os << "Operator"; break;
			case Token::Type::EQUAL: os << "Equal"; break;
			case Token::Type::BRACKET_OPEN: os << "Bracket open"; break;
			case Token::Type::BRACKET_MATRIX_OPEN: os << "Bracket matrix open"; break;
			case Token::Type::BRACKET_CLOSE: os << "Bracket close"; break;
			case Token::Type::BRACKET_MATRIX_CLOSE: os << "Bracket matrix close"; break;
			case Token::Type::COMMA: os << "Comma"; break;
			case Token::Type::SEMICOLON: os << "Semicolon"; break;
			case Token::Type::QUERY: os << "Query"; break;
			default: os << "Unkown Token"; break;
		}
		os << " ";
		switch (token.op)
		{
			case Token::Operator::ADD: os << "Add"; break;
			case Token::Operator::SUB: os << "Sub"; break;
			case Token::Operator::MUL: os << "Mul"; break;
			case Token::Operator::DIV: os << "Div"; break;
			case Token::Operator::MOD: os << "Mod"; break;
			case Token::Operator::POW: os << "Pow"; break;
			case Token::Operator::DOT: os << "Dot"; break;
			default: break;
		}
		if (!token.value.empty())
			os << "(\"" << token.value << "\")";
		os << '\n';
		return os;
	}
};
