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

NodePtr	Parser::parseMatrix()
{
    stream.consume(TokenType::BRACKET_MATRIX_OPEN);

    std::size_t             rows = 0;
    std::size_t             cols = 0;
    std::vector<NodePtr>    elements;

    auto parseRow = [&]()
    {
        stream.consume(TokenType::BRACKET_MATRIX_OPEN);
        std::size_t count = 0;
        elements.push_back(parseExpression());
        ++count;
        while (stream.match(TokenType::COMMA))
        {
            elements.push_back(parseExpression());
            ++count;
        }
        stream.consume(TokenType::BRACKET_MATRIX_CLOSE);
        return count;
    };

    cols = parseRow();
    ++rows;

	while (stream.match(TokenType::SEMICOLON))
	{
        std::size_t currentCols = parseRow();
        if (currentCols != cols) throw std::runtime_error("Error: Inconsistent row size in matrix definition.");
        ++rows;
    }
	stream.consume(TokenType::BRACKET_MATRIX_CLOSE);
	return std::make_unique<MatrixNode>(rows, cols, std::move(elements));
}
