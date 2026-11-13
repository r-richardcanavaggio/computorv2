#include "Interpreter/Interpreter.hpp"

void	Interpreter::commandList( const Token& ) const
{
	if (ctx.empty())
		std::cout << "No Variables.\n";
	else
	{
		for (const auto& pair : ctx)
			std::cout << pair.first << ": " << pair.second << "\n";
	}
}

void	Interpreter::commandHistory( const Token& ) const
{
	if (history.empty())
		std::cout << "History is empty.\n";
	else
	{
		for (std::size_t i = 0; i < history.size(); i++)
			std::cout << i + 1 << ": " << history[i] << "\n";
	}
}

void	Interpreter::commandDeg( const Token& )
{
	ctx.isDegreeMode = true;

	std::cout << "Angle mode switched to: DEGREES" << std::endl;
}

void	Interpreter::commandRad( const Token& )
{
	ctx.isDegreeMode = false;

	std::cout << "Angle mode switched to: RADIANS" << std::endl;
}

void	Interpreter::commandExit( const Token& )
{
	std::cout << "Exiting ComputorV2, Goodbye!\n";
	running = false;
}

void	Interpreter::commandHelp( const Token& ) const
{
	std::cout << "=========================================\n"
			<< "          ComputorV2 - Help Menu         \n"
			<< "=========================================\n"
			<< "  list         - List all saved variables and functions\n"
			<< "  history      - Show the command history\n"
			<< "  clear <var>  - Delete a specific variable or function\n"
			<< "  deg / rad    - Switch angle mode for trigonometry\n"
			<< "  help         - Show this help message\n"
			<< "  exit / quit  - Exit the program\n"
			<< "-----------------------------------------\n"
			<< "Built-in functions:\n"
			<< "  cos, sin, tan, sqrt, abs, exp, norm, inv\n"
			<< "=========================================" 
			<< "\n";
}

void	Interpreter::commandClear( const Token& arg )
{
	if (arg.type != Token::Type::VARIABLE)
		throw std::runtime_error("Usage: clear <variable>\n");
	
	auto	it = ctx.find(arg.value);

	if (it == ctx.end())
		throw std::runtime_error("Error: '" + arg.value + "' is not defined.\n");

	ctx.erase(it);
	std::cout << "Variable/Function '" << arg.value << "' has been deleted.\n";
}
