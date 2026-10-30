#include "Interpreter/Interpreter.hpp"
#include "Interpreter/Evaluator.hpp"
#include <regex>

namespace Lexer
{

using TokenDef = std::pair<TokenType, OpKind>;

Token	createSymbolToken( const std::string& str )
{
	static const std::map<std::string, TokenDef>	symbolMap = {
		{"=",  {TokenType::EQUAL,				 OpKind::NONE}},
		{"[",  {TokenType::BRACKET_MATRIX_OPEN,	 OpKind::NONE}},
		{"]",  {TokenType::BRACKET_MATRIX_CLOSE, OpKind::NONE}},
		{"(",  {TokenType::BRACKET_OPEN,		 OpKind::NONE}},
		{")",  {TokenType::BRACKET_CLOSE,		 OpKind::NONE}},
		{",",  {TokenType::COMMA,				 OpKind::NONE}},
		{";",  {TokenType::SEMICOLON,			 OpKind::NONE}},
		{"?",  {TokenType::QUERY,				 OpKind::NONE}},
		{"+",  {TokenType::OPERATOR,			 OpKind::ADD}},
		{"-",  {TokenType::OPERATOR,			 OpKind::SUB}},
		{"**", {TokenType::OPERATOR,			 OpKind::DOT}},
		{"*",  {TokenType::OPERATOR,			 OpKind::MUL}},
		{"/",  {TokenType::OPERATOR,			 OpKind::DIV}},
		{"%",  {TokenType::OPERATOR,			 OpKind::MOD}},
		{"^",  {TokenType::OPERATOR,			 OpKind::POW}},
	};

	auto	it = symbolMap.find(str);

	if (it != symbolMap.end())
		return (Token(str, it->second.first, Arity::CONSTANT, it->second.second));
	return (Token(str, TokenType::UNKOWN, Arity::CONSTANT, OpKind::NONE));
}

std::vector<Token>	tokenize( const std::string& input )
{
	static const std::regex 	pattern("([0-9]+(\\.[0-9]+)?)|([a-zA-Z][a-zA-Z0-9_]*)|(\\*\\*|[\\+\\-\\*\\/%\\^\\(\\)=])|([\\[\\]\\,\\;])|(\\?)");
	auto						words_begin = std::sregex_iterator(input.begin(), input.end(), pattern);
	auto						words_end	= std::sregex_iterator();
	std::vector<Token>			tokens;

	for (std::sregex_iterator i = words_begin; i != words_end; i++)
	{
		std::string	match_str = i->str();

		if (isdigit(match_str[0]))
			tokens.emplace_back(match_str, TokenType::NUMBER, Arity::CONSTANT, OpKind::NONE);
		else if (isalpha(match_str[0]))
		{
			for (auto& c : match_str)
				c = std::tolower(static_cast<unsigned char>(c));
			TokenType	type = (match_str == "i") ? TokenType::IMAGINARY : TokenType::VARIABLE;

			tokens.emplace_back(match_str, type, Arity::CONSTANT, OpKind::NONE);
		}
		else
			tokens.push_back(createSymbolToken(match_str));
	}
	return (tokens);
}

}

Interpreter::Interpreter() : running(true)
{
	ctx["pi"] = Real(maths::pi);
	ctx["e"] = Real(maths::e);
	ctx["sqrt2"] = Real(maths::sqrt2);
	ctx["i"] = Complex(0, 1);

	commands = {
		{"list",    [this](std::span<const Token> args) { commandList(args); }},
		{"clear",   [this](std::span<const Token> args) { commandClear(args); }},
		{"history", [this](std::span<const Token> args) { commandHistory(args); }},
		{"deg",     [this](std::span<const Token> args) { commandDeg(args); }},
		{"rad",     [this](std::span<const Token> args) { commandRad(args); }},
		{"exit",    [this](std::span<const Token> args) { commandExit(args); }},
		{"quit",    [this](std::span<const Token> args) { commandExit(args); }},
		{"help",    [this](std::span<const Token> args) { commandHelp(args); }}
		// {"plot",    [this](std::span<const Token> args) { commandPlot(args); }}
	};
}

bool	Interpreter::isRunning() const
{
	return running;
}

bool	Interpreter::executeCommand( const std::vector<Token>& tokens )
{
	if (tokens.empty())
		return false;
	
	auto	cmd = tokens[0].value;
	auto	it = commands.find(cmd);

	if (it == commands.end())
		return false;
	
	std::span<const Token>	args = std::span(tokens).subspan(1);

	it->second(args);
	return true;
}

void	Interpreter::processLine( const std::string& line )
{
	try
	{
		auto	tokens = Lexer::tokenize(line);
		if (tokens.empty()) return ;

		if (executeCommand(tokens)) return ;

		Evaluator evaluator(ctx, history, std::move(tokens), line);
		evaluator.evaluate();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
