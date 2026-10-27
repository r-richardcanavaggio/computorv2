#include "Parser/Parser.hpp"

NodePtr Parser::parseNumber()
{
    const Token&    token = stream.advance();
    return std::make_unique<NumberNode>(Real(token.value));
}

NodePtr Parser::parseImaginary()
{
    stream.advance();
    return std::make_unique<ImaginaryNode>();
}

NodePtr Parser::parseParenthesized()
{
    stream.advance();
    auto    expr = parseExpression();
    stream.consume(TokenType::BRACKET_CLOSE);
    return expr;
}

NodePtr Parser::parseVariableOrCall()
{
    std::string name = stream.advance().value;

    if (stream.match(TokenType::BRACKET_OPEN))
    {
        auto expr = parseExpression();
        stream.consume(TokenType::BRACKET_CLOSE);

        if (isBuiltinFunction(name))
            return std::make_unique<BuiltinFunctionNode>(name, std::move(expr));
        return std::make_unique<FunctionCallNode>(name, std::move(expr));
    }
    return std::make_unique<VariableNode>(name);
}

std::vector<NodePtr>	Parser::parseMatrixRow()
{
	stream.consume(TokenType::BRACKET_MATRIX_OPEN);

	std::vector<NodePtr>	row;
	row.push_back(parseExpression());

	while (stream.match(TokenType::COMMA))
		row.push_back(parseExpression());
	
	stream.consume(TokenType::BRACKET_MATRIX_CLOSE);
	return row;
}

NodePtr	Parser::parseMatrix()
{
    stream.consume(TokenType::BRACKET_MATRIX_OPEN);

	std::vector<std::vector<NodePtr>>	rows;
	rows.push_back(parseMatrixRow());

	while (stream.match(TokenType::SEMICOLON))
		rows.push_back(parseMatrixRow());
	
	stream.consume(TokenType::BRACKET_MATRIX_CLOSE);

	return std::make_unique<MatrixNode>(std::move(rows));
}
