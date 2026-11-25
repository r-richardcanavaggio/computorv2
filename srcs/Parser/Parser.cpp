#include "Parser/Parser.hpp"
#include <unordered_set>

bool Parser::isBuiltinFunction( std::string_view name )
{
	static const std::unordered_set<std::string_view> builtins = {
		"cos", "sin", "tan", "sqrt", "abs", "exp", "inv", "norm"};
	return (builtins.contains(name));
}

NodePtr Parser::parsePrimary()
{
	switch (stream.peek().type)
	{
		case Token::Type::NUMBER:
			return parseNumber();
		case Token::Type::VARIABLE:
			return parseVariableOrCall();
		case Token::Type::IMAGINARY:
			return parseImaginary();
		case Token::Type::BRACKET_OPEN:
			return parseParenthesized();
		case Token::Type::BRACKET_MATRIX_OPEN:
			return parseMatrix();
		default:
			throw std::runtime_error("Unexpected token: '" + std::string(stream.peek().toString()) + "'");
	}
}

NodePtr Parser::parsePower()
{
	auto left = parsePrimary();

	if (stream.matchOp(Token::Operator::POW))
	{
		auto right = parseUnary();
		return (std::make_unique<BinaryOpNode>(Token::Operator::POW, std::move(left), std::move(right)));
	}
	return (left);
}

NodePtr Parser::parseUnary()
{
	if (auto op = stream.matchAnyOp({Token::Operator::ADD, Token::Operator::SUB}))
	{
		auto	right = parsePower();
		return std::make_unique<UnaryOpNode>(*op, std::move(right));
	}
	return parsePower();
}

NodePtr Parser::parseTerm()
{
	auto left = parseUnary();

	while (auto op = stream.matchAnyOp({Token::Operator::MUL, Token::Operator::DIV, Token::Operator::MOD, Token::Operator::DOT}))
	{
		auto right = parseUnary();
		left = std::make_unique<BinaryOpNode>(*op, std::move(left), std::move(right));
	}
	return (left);
}

NodePtr Parser::parseExpression()
{
	auto left = parseTerm();

	while (auto op = stream.matchAnyOp({Token::Operator::ADD, Token::Operator::SUB}))
	{
		auto right = parseTerm();
		left = std::make_unique<BinaryOpNode>(*op, std::move(left), std::move(right));
	}
	return (left);
}

NodePtr Parser::parse()
{
	auto ast = parseExpression();

	if (!stream.isAtEnd())
		throw std::runtime_error("Unexpected token at end of expression: '" + std::string(stream.peek().toString()) + "'");
	return (ast);
}
