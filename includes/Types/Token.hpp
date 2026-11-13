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

	Type		type;
	Operator	op;
	std::string	value;
	Arity		arity;

	Token( Type t, Operator o = Operator::NONE, std::string sv = "", Arity a = Arity::CONSTANT )
		: type(t), op(o), value(sv), arity(a) {}
	
	friend std::ostream& operator<<( std::ostream& os, const Token& token )
	{
		switch (token.type)
		{
			case Token::Type::NUMBER: os << "NUMBER"; break;
			case Token::Type::VARIABLE: os << "VARIABLE"; break;
			case Token::Type::IMAGINARY: os << "IMAGINARY"; break;
			case Token::Type::OPERATOR: os << "OP"; break;
			case Token::Type::EQUAL: os << "EQ"; break;
			case Token::Type::BRACKET_OPEN: os << "BRACKET_OPEN"; break;
			case Token::Type::BRACKET_MATRIX_OPEN: os << "BRACKET_MATRIX_OPEN"; break;
			case Token::Type::BRACKET_CLOSE: os << "BRACKET_CLOSE"; break;
			case Token::Type::BRACKET_MATRIX_CLOSE: os << "BRACKET_MATRIX_CLOSE"; break;
			case Token::Type::COMMA: os << "COMMA"; break;
			case Token::Type::SEMICOLON: os << "SEMICOLON"; break;
			case Token::Type::QUERY: os << "QUERY"; break;
			default: os << "UNKOWN"; break;
		}
		os << " ";
		switch (token.op)
		{
			case Token::Operator::ADD: os << "ADD"; break;
			case Token::Operator::SUB: os << "SUB"; break;
			case Token::Operator::MUL: os << "MUL"; break;
			case Token::Operator::DIV: os << "DIV"; break;
			case Token::Operator::MOD: os << "MOD"; break;
			case Token::Operator::POW: os << "POW"; break;
			case Token::Operator::DOT: os << "DOT"; break;
			default: break;
		}
		os << '\n';
		return os;
	}
};
