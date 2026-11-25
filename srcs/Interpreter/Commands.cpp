#include "Interpreter/Interpreter.hpp"

void	Interpreter::commandList( const Token& ) const
{
	std::cout << "=========================================\n"
			  << "              Saved Context              \n"
			  << "=========================================\n"
			  << "Variables:\n";
	if (ctx.vars.empty())
		std::cout << "  (none)\n";
	else
	{
		for (const auto& pair : ctx.vars)
		{
			std::cout << "  " << pair.first << ": ";
			std::visit([]( const auto& arg ) { arg.type(); }, pair.second);
			std::cout << " = " << pair.second << '\n';
		}
	}
	std::cout << "-----------------------------------------\n"
			  << "Functions:\n";
	if (ctx.funcs.empty())
		std::cout << "  (none)\n";
	else
	{
		for (const auto& pair : ctx.funcs)
		{
			std::cout << "  " << pair.first << ": ";
			std::visit([]( const auto& arg ) { arg.type(); }, pair.second);
			std::cout << " = " << pair.second << '\n';
		}
	}
	std::cout << "=========================================\n";
}

void	Interpreter::commandHistory( const Token& ) const
{
	if (history.empty())
		std::cout << "Info: History is empty.\n";
	else
	{
		for (std::size_t i = 0; i < history.size(); i++)
			std::cout << i + 1 << ": " << history[i] << "\n";
	}
}

void	Interpreter::commandDeg( const Token& )
{
	ctx.isDegreeMode = true;

	std::cout << "Info: Angle mode switched to: DEGREES\n";
}

void	Interpreter::commandRad( const Token& )
{
	ctx.isDegreeMode = false;

	std::cout << "Info: Angle mode switched to: RADIANS\n";
}

void	Interpreter::commandExit( const Token& )
{
	std::cout << "Exiting ComputorV2, Goodbye!\n";
	running = false;
}

void	Interpreter::commandHelp( const Token& ) const
{
	std::cout << "====================================================================\n"
			  << "                       ComputorV2 - Help Menu         \n"
			  << "====================================================================\n"
			  << "  list              - List all saved variables and functions\n"
			  << "  history           - Show the command history\n"
			  << "  clear <var/func>  - Delete a specific variable or function\n"
			  << "  deg / rad         - Switch angle mode for trigonometry\n"
			  << "  help              - Show this help message\n"
			  << "  exit / quit       - Exit the program\n"
			  << "--------------------------------------------------------------------\n"
			  << "Built-in functions:\n"
			  << "  cos, sin, tan, sqrt, abs, exp, norm, inv\n"
			  << "====================================================================" 
			  << "\n";
}

void	Interpreter::commandClear( const Token& arg )
{
	if (arg.type != Token::Type::VARIABLE)
		throw std::runtime_error("Usage: clear <variable>\n");

	auto	it = ctx.vars.find(arg.value);

	if (it != ctx.vars.end())
	{
		ctx.vars.erase(it);
		std::cout << "Info: Variable '" << arg.value << "' has been deleted.\n";
		return ;
	}

	auto	funcIt = ctx.funcs.find(arg.value);

	if (funcIt == ctx.funcs.end())
		throw std::runtime_error("Error: '" + arg.value + "' is not defined.");

	ctx.funcs.erase(funcIt);
	std::cout << "Info: Function '" << arg.value << "' has been deleted.\n";
}
