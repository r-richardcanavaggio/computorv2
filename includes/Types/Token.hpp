#pragma once 

#include <string>

enum class TokenType
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
	CONSTANT,
	UNARY,
	BINARY
};

enum class OpKind
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

struct Token
{
	std::string	value;
	TokenType	type;
	Arity		arity;
	OpKind		op;

	Token( const std::string& v, TokenType t, Arity a, OpKind o ) : value(v), type(t), arity(a), op(o) {}
};
