#include "Parser/TokenStream.hpp"

TokenStream::TokenStream( std::span<const Token> toks ) : tokens(toks), pos(0) {}

bool	TokenStream::isAtEnd() const
{
	return pos >= tokens.size();
}

bool	TokenStream::check( TokenType type ) const
{
	return !isAtEnd() && peek().type == type;
}

const Token&	TokenStream::peek() const
{
	if (isAtEnd())
		throw std::runtime_error("Unexpected end of input");
	return (tokens[pos]);
}

const Token&	TokenStream::advance()
{
	if (isAtEnd())
		throw std::runtime_error("Unexpected end of input");
	return (tokens[pos++]);
}

const Token&	TokenStream::consume( TokenType expected )
{
	if (isAtEnd())
		throw std::runtime_error("Unexpected end of input");
	if (peek().type != expected)
		throw std::runtime_error("Unexpected token: '" + peek().value + "'");
	return (advance());
}

bool	TokenStream::match( TokenType type )
{
	if (!isAtEnd() && peek().type == type)
	{
		advance();
		return (true);
	}
	return (false);
}

bool	TokenStream::matchOp( OpKind op )
{
	if (!isAtEnd() && peek().type == TokenType::OPERATOR && peek().op == op)
	{
		advance();
		return (true);
	}
	return (false);
}

std::optional<OpKind>	TokenStream::matchAnyOp( std::initializer_list<OpKind> ops )
{
	if (!isAtEnd() && peek().type == TokenType::OPERATOR)
	{
		for (OpKind op : ops)
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