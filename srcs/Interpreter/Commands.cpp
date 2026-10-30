#include "Interpreter.hpp"
#include "Plotter.hpp"

void	Interpreter::commandList( std::span<const Token> args ) const
{
	if (!args.empty())
		throw std::runtime_error("Usage: list (takes no arguments)\n");

	if (ctx.empty())
		std::cout << "No Variables.\n";
	else
	{
		for (const auto& pair : ctx)
			std::cout << pair.first << ": " << pair.second << "\n";
	}
}

void	Interpreter::commandHistory( std::span<const Token> args ) const
{
	if (!args.empty())
		throw std::runtime_error("Usage: history (takes no arguments)\n");

	if (history.empty())
		std::cout << "History is empty.\n";
	else
	{
		for (std::size_t i = 0; i < history.size(); i++)
			std::cout << i + 1 << ": " << history[i] << "\n";
	}
}

void	Interpreter::commandDeg( std::span<const Token> args )
{
	if (!args.empty())
		throw std::runtime_error("Usage: deg (takes no arguments)\n");

	ctx.isDegreeMode = true;

	std::cout << "Angle mode switched to: DEGREES" << std::endl;
}

void	Interpreter::commandRad( std::span<const Token> args)
{
	if (!args.empty())
		throw std::runtime_error("Usage: rad (takes no arguments)\n");

	ctx.isDegreeMode = false;

	std::cout << "Angle mode switched to: RADIANS" << std::endl;
}

void	Interpreter::commandExit( std::span<const Token> args )
{
	if (!args.empty())
		throw std::runtime_error("Usage: exit/quit (takes no arguments)\n");

	std::cout << "Exiting ComputorV2, Goodbye!\n";
	running = false;
}

void	Interpreter::commandHelp( std::span<const Token> args ) const
{
	if (!args.empty())
		throw std::runtime_error("Usage: Help (takes no arguments)\n");

	std::cout << "=========================================\n"
			<< "          ComputorV2 - Help Menu         \n"
			<< "=========================================\n"
			<< "  list         - List all saved variables and functions\n"
			<< "  history      - Show the command history\n"
			<< "  clear <var>  - Delete a specific variable or function\n"
			<< "  plot <func>  - Plot a function in an ASCII graph\n"
			<< "  deg / rad    - Switch angle mode for trigonometry\n"
			<< "  help         - Show this help message\n"
			<< "  exit / quit  - Exit the program\n"
			<< "-----------------------------------------\n"
			<< "Built-in functions:\n"
			<< "  cos, sin, tan, sqrt, abs, exp, norm, inv\n"
			<< "=========================================" 
			<< "\n";
}

void	Interpreter::commandClear( std::span<const Token> args )
{
	if (args.size() != 1 || args[0].type != TokenType::VARIABLE)
		throw std::runtime_error("Usage: clear <variable>\n");
	
	auto	varToDelete = args[0].value;
	auto	it = ctx.find(varToDelete);

	if (it == ctx.end())
		throw std::runtime_error("Error: '" + varToDelete + "' is not defined.\n");

	ctx.erase(it);
	std::cout << "Variable/Function '" << varToDelete << "' has been deleted.\n";
}

// void	Interpreter::commandPlot( std::span<const Token> args )
// {
// 	auto	targetFunc = args[0].value;
// 	auto	it = ctx.find(targetFunc);

// 	if (it == ctx.end())
// 		throw std::runtime_error("Error: Unknown function '" + targetFunc + "'.\n");
// 	if (!std::holds_alternative<Polynomial>(it->second))
// 		throw std::runtime_error("Error: '" + targetFunc + "' is not a function/polynomial.\n");

// 	Plotter		canvas(80, 60);
// 	Polynomial	f = std::get<Polynomial>(it->second); 

// 	canvas.drawLine(0, 30, 80, 30);
// 	canvas.drawLine(40, 0, 40, 60);

// 	for (float x = -20.0; x <= 20.0; x += 0.1)
// 	{
// 		double	y = f.eval(Real(x)).getValue();
// 		int	px = (x + 20) * 2;
// 		int	py = (y + 30);
// 		canvas.set(px, py);
// 	}
// 	canvas.display();
// }
