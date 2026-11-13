#include "Parser/TokenStream.hpp"

TokenStream::TokenStream( std::span<const Token> toks ) : tokens(toks), pos(0) {}

bool TokenStream::isAtEnd() const
{
	return pos >= tokens.size();
}

bool TokenStream::check( Token::Type type ) const
{
	return !isAtEnd() && peek().type == type;
}

const Token& TokenStream::peek() const
{
	if (isAtEnd())
		throw std::runtime_error("Unexpected end of input");
	return (tokens[pos]);
}

const Token& TokenStream::advance()
{
	if (isAtEnd())
		throw std::runtime_error("Unexpected end of input");
	return (tokens[pos++]);
}

const Token& TokenStream::consume( Token::Type expected )
{
	if (isAtEnd())
		throw std::runtime_error("Unexpected end of input");
	if (peek().type != expected)
		throw std::runtime_error("Unexpected token: '" + peek().value + "'");
	return (advance());
}

bool TokenStream::match( Token::Type type )
{
	if (!isAtEnd() && peek().type == type)
	{
		advance();
		return (true);
	}
	return (false);
}

bool TokenStream::matchOp( Token::Operator op )
{
	if (!isAtEnd() && peek().type == Token::Type::OPERATOR && peek().op == op)
	{
		advance();
		return (true);
	}
	return (false);
}

std::optional<Token::Operator> TokenStream::matchAnyOp( std::initializer_list<Token::Operator> ops )
{
	if (!isAtEnd() && peek().type == Token::Type::OPERATOR)
	{
		for (Token::Operator op : ops)
		{
			if (peek().op == op)
			{
				advance();
				return op;
			}
		}
	}
	return std::nullopt;
}