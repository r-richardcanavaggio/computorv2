#include "Interpreter.hpp"
#include "Plotter.hpp"
#include "Evaluator.hpp"
#include "Maths.hpp"

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
		{"help",    [this](std::span<const Token> args) { commandHelp(args); }},
		{"plot",    [this](std::span<const Token> args) { commandPlot(args); }}
	};
}

Interpreter::~Interpreter() {}

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
		auto	tokens = lexer.tokenize(line);
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
