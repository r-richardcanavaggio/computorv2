#include "Parser/Parser.hpp"
#include <unordered_set>

bool	Parser::isBuiltinFunction( std::string_view name )
{
	static const std::unordered_set<std::string_view>	builtins = {
		"cos", "sin", "tan", "sqrt", "abs", "exp", "inv", "norm"
	};
	return (builtins.contains(name));
}

NodePtr	Parser::parsePrimary()
{
	switch (stream.peek().type)
	{
		case TokenType::NUMBER:
			return parseNumber();
		case TokenType::VARIABLE:
			return parseVariableOrCall();
		case TokenType::IMAGINARY:
			return parseImaginary();
		case TokenType::BRACKET_OPEN:
			return parseParenthesized();
		case TokenType::BRACKET_MATRIX_OPEN:
			return parseMatrix();
		default:
			throw std::runtime_error("Unexpected token: '" + stream.peek().value + "'");
	}
}

NodePtr	Parser::parsePower()
{
	auto	left = parsePrimary();

	if (stream.matchOp(OpKind::POW))
	{
		auto	right = parseFactor();
		return (std::make_unique<BinaryOpNode>(OpKind::POW, std::move(left), std::move(right)));
	}
	return (left);
}

NodePtr	Parser::parseFactor()
{
	if (stream.matchOp(OpKind::ADD))
		return (parseFactor());
	if (stream.matchOp(OpKind::SUB))
		return (std::make_unique<UnaryOpNode>(OpKind::SUB, parseFactor()));
	return (parsePower());
}

NodePtr	Parser::parseTerm()
{
	auto	left = parseFactor();

	while (auto op = stream.matchAnyOp({OpKind::MUL, OpKind::DIV, OpKind::MOD, OpKind::MUL_MAT}))
	{
		auto	right = parseFactor();
		left = std::make_unique<BinaryOpNode>(*op, std::move(left), std::move(right));
	}
	return (left);
}

NodePtr	Parser::parseExpression()
{
	auto	left = parseTerm();

	while (auto op = stream.matchAnyOp({OpKind::ADD, OpKind::SUB}))
	{
		auto	right = parseTerm();
		left = std::make_unique<BinaryOpNode>(*op, std::move(left), std::move(right));
	}
	return (left);
}

NodePtr	Parser::parse()
{
	auto	ast = parseExpression();

	if (!stream.isAtEnd())
		throw std::runtime_error("Unexpected token at end of expression: '" + stream.peek().value + "'");
	return (ast);
}
