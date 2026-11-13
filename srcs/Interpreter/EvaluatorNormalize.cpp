#include "Interpreter/Evaluator.hpp"

bool	Evaluator::isEndExpression( const Token& token )
{
	return (token.type == Token::Type::NUMBER ||
			token.type == Token::Type::VARIABLE ||
			token.type == Token::Type::IMAGINARY ||
			token.type == Token::Type::BRACKET_CLOSE);
}

bool	Evaluator::isStartExpression( const Token& token )
{
	return (token.type == Token::Type::NUMBER ||
			token.type == Token::Type::VARIABLE ||
			token.type == Token::Type::IMAGINARY ||
			token.type == Token::Type::BRACKET_OPEN);
}

void	Evaluator::resolveOperatorArity()
{
	bool expects_operands = true;

	for (auto &token : tokens)
	{
		if (token.type == Token::Type::VARIABLE || token.type == Token::Type::NUMBER || token.type == Token::Type::IMAGINARY)
			expects_operands = false;
		else if (token.type == Token::Type::BRACKET_OPEN)
			expects_operands = true;
		else if (token.type == Token::Type::BRACKET_CLOSE)
			expects_operands = false;
		else if (token.type == Token::Type::OPERATOR)
		{
			token.arity = expects_operands ? Token::Arity::UNARY : Token::Arity::BINARY;
			expects_operands = true;
		}
	}
}

void	Evaluator::insertImplicitMultiplications()
{
	std::size_t i = 0;

	while (i + 1 < tokens.size())
	{
		if (isEndExpression(tokens[i]) && isStartExpression(tokens[i + 1]) &&
			!(tokens[i].type == Token::Type::VARIABLE && tokens[i + 1].type == Token::Type::BRACKET_OPEN))
		{
			tokens.insert(tokens.begin() + (i + 1), Token(Token::Type::OPERATOR, Token::Operator::MUL, "", Token::Arity::BINARY));
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
