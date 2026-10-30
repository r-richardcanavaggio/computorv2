#include "Interpreter/Evaluator.hpp"

bool	Evaluator::isEndExpression( const Token& token )
{
	return (token.type == TokenType::NUMBER    ||
			token.type == TokenType::VARIABLE  || 
			token.type == TokenType::IMAGINARY || 
			token.type == TokenType::BRACKET_CLOSE);
}

bool	Evaluator::isStartExpression( const Token& token )
{
	return (token.type == TokenType::NUMBER    ||
			token.type == TokenType::VARIABLE  ||
			token.type == TokenType::IMAGINARY ||
			token.type == TokenType::BRACKET_OPEN);
}

void	Evaluator::resolveOperatorArity()
{
	bool	expects_operands = true;

	for (auto& token : tokens)
	{
		if (token.type == TokenType::VARIABLE || token.type == TokenType::NUMBER || token.type == TokenType::IMAGINARY)
			expects_operands = false;
		else if (token.type == TokenType::BRACKET_OPEN)
			expects_operands = true;
		else if (token.type == TokenType::BRACKET_CLOSE)
			expects_operands = false;
		else if (token.type == TokenType::OPERATOR)
		{
			token.arity = expects_operands ? Arity::UNARY : Arity::BINARY;
			expects_operands = true;
		}
	}
}

void	Evaluator::insertImplicitMultiplications()
{
	std::size_t	i = 0;

	while (i + 1 < tokens.size())
	{
		if (isEndExpression(tokens[i])              &&
			isStartExpression(tokens[i + 1])        &&
			!(tokens[i].type == TokenType::VARIABLE &&
			tokens[i + 1].type == TokenType::BRACKET_OPEN))
		{
			tokens.insert(tokens.begin() + (i + 1), Token("*", TokenType::OPERATOR, Arity::BINARY, OpKind::MUL));
			i++;
		}
		else
			i++;
	}
}

void	Evaluator::normalize()
{
	resolveOperatorArity();
	insertImplicitMultiplications();
}
