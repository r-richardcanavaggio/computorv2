#include "Interpreter/Interpreter.hpp"
#include "Interpreter/Evaluator.hpp"
#include <regex>

namespace Lexer
{

bool	isVarStart( char c )
{
	return std::isalpha(static_cast<unsigned char>(c));
}

bool	isVarChar( char c )
{
	return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

std::string	scanVariable( std::string_view src, std::size_t& i )
{
	if (i >= src.size())
		return {};

	std::size_t	start = i;
	++i;
	while (i < src.size() && isVarChar(src[i]))
		++i;
	return std::string(src.substr(start, i - start));
}

bool	isNumberStart( char c )
{
	return std::isdigit(static_cast<unsigned char>(c));
}

bool	isNumberChar( char c )
{
	return std::isdigit(static_cast<unsigned char>(c)) || c == '.';
}

std::string	scanNumber( std::string_view src, std::size_t& i )
{
	if (i >= src.size() || !isNumberStart(src[i]))
		return {};
	
	bool		dot = false;
	std::size_t	start = i;
	++i;
	while (i < src.size() && isNumberChar(src[i]))
	{
		++i;
		if (dot && src[i] == '.')
			return std::string(src.substr(start, i - start));
		if (src[i] == '.')
			dot = true;
	}
	return std::string(src.substr(start, i - start));
}

void	addToken( std::vector<Token>& tokens, Token tok, std::size_t& i )
{
	tokens.push_back(tok);
	++i;
}

std::vector<Token> tokenize( std::string_view input )
{
	std::vector<Token>	tokens;
	std::size_t			i = 0;

	while (i < input.length())
	{
		char c = input[i];
		switch (c)
		{
			case '-': addToken(tokens, {Token::Type::OPERATOR, Token::Operator::SUB}, i); break;
			case '+': addToken(tokens, {Token::Type::OPERATOR, Token::Operator::ADD}, i); break;
			case '/': addToken(tokens, {Token::Type::OPERATOR, Token::Operator::DIV}, i); break;
			case '%': addToken(tokens, {Token::Type::OPERATOR, Token::Operator::MOD}, i); break;
			case '^': addToken(tokens, {Token::Type::OPERATOR, Token::Operator::POW}, i); break;
			case '=': addToken(tokens, {Token::Type::EQUAL}, i); break;
			case '(': addToken(tokens, {Token::Type::BRACKET_OPEN}, i); break;
			case ')': addToken(tokens, {Token::Type::BRACKET_CLOSE}, i); break;
			case '[': addToken(tokens, {Token::Type::BRACKET_MATRIX_OPEN}, i); break;
			case ']': addToken(tokens, {Token::Type::BRACKET_MATRIX_CLOSE}, i); break;
			case ',': addToken(tokens, {Token::Type::COMMA}, i); break;
			case ';': addToken(tokens, {Token::Type::SEMICOLON}, i); break;
			case '?': addToken(tokens, {Token::Type::QUERY}, i); break;
			case 'i': addToken(tokens, {Token::Type::IMAGINARY}, i); break;
			case '*':
			{
				++i;
				if (i < input.size() && c == '*')
					addToken(tokens, {Token::Type::OPERATOR, Token::Operator::DOT}, i);
				else
					addToken(tokens, {Token::Type::OPERATOR, Token::Operator::MUL}, i);
				break;
			}
			default:
			{
				if (isVarStart(c))
				{
					std::string	var = scanVariable(input, i);
					if (!var.empty())
						tokens.push_back({Token::Type::VARIABLE, Token::Operator::NONE, var});
					break;
				}
				else if (isNumberStart(c))
				{
					std::string num = scanNumber(input, i);
					if (!num.empty())
						tokens.push_back({Token::Type::NUMBER, Token::Operator::NONE, num});
					break;
				}
				else if (std::isspace(c))
					++i;
				else
					addToken(tokens, {Token::Type::UNKOWN, Token::Operator::NONE, std::string(&c)}, i);
				break;
			}
		}
	}
	return tokens;
}

}

Interpreter::Interpreter() : running(true)
{
	ctx["pi"] = Real(maths::pi);
	ctx["e"] = Real(maths::e);
	ctx["sqrt2"] = Real(maths::sqrt2);
	ctx["i"] = Complex(0, 1);

	commands = {
		{"list",    [this]( const Token& arg ) { commandList(arg); }},
		{"clear",   [this]( const Token& arg ) { commandClear(arg); }},
		{"history", [this]( const Token& arg ) { commandHistory(arg); }},
		{"deg",     [this]( const Token& arg ) { commandDeg(arg); }},
		{"rad",     [this]( const Token& arg ) { commandRad(arg); }},
		{"exit",    [this]( const Token& arg ) { commandExit(arg); }},
		{"quit",    [this]( const Token& arg ) { commandExit(arg); }},
		{"help",    [this]( const Token& arg ) { commandHelp(arg); }}
	};
}

bool	Interpreter::isRunning() const
{
	return running;
}

bool	Interpreter::executeCommand( std::span<const Token> tokens )
{
	if (tokens.empty())
		return false;
	
	auto	it = commands.find(tokens.front().value);

	if (it != commands.end())
	{
		if (tokens.size() > 2)
			throw std::runtime_error("Usage: <Command> (argument).");

		it->second(tokens.back());
		return true;
	}
	return false;
}

void	Interpreter::processLine( const std::string& line )
{
	try
	{
		std::vector<Token>	tokens = Lexer::tokenize(line);

		if (tokens.empty())
			return ;

		if (executeCommand(tokens))
			return ;

		Evaluator evaluator(ctx, history, std::move(tokens), line);

		evaluator.evaluate();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
