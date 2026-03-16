
#include "InterpreterCommandHandler.hpp"
#include "Plotter.hpp"
#include <iostream>

namespace interpreter_cmd
{
	bool	handle( const std::vector<Token>& tokens, Context& ctx, const std::vector<std::string>& history )
	{
		if (tokens.empty())
			return (false);

		const std::string&	cmd = tokens[0].value;

		if (tokens.size() == 1 && cmd == "list")
		{
			if (ctx.empty())
				std::cout << "No Variables.\n";
			else
			{
				for (const auto& pair : ctx)
					std::cout << pair.first << " : " << pair.second << std::endl;
			}
			return (true);
		}
		if (tokens.size() == 1 && cmd == "history")
		{
			if (history.empty())
				std::cout << "History is empty." << std::endl;
			else
			{
				for (std::size_t i = 0; i < history.size(); i++)
					std::cout << i + 1 << ": " << history[i] << std::endl;
			}
			return (true);
		}
		if (cmd == "clear")
		{
			if (tokens.size() != 2 || tokens[1].type != TokenType::VARIABLE)
			{
				std::cout << "Usage: clear <variable>\n";
				return (true);
			}

			const std::string&	varToDelete = tokens[1].value;
			auto				it = ctx.find(varToDelete);

			if (it != ctx.end())
			{
				ctx.erase(it);
				std::cout << "Variable/Function '" << varToDelete << "' has been deleted.\n";
			}
			else
				std::cout << "Error: '" << varToDelete << "' is not defined.\n";
			return (true);
		}
		if (tokens.size() == 2 && cmd == "plot" && tokens[1].type == TokenType::VARIABLE)
		{
			const std::string&	targetFunc = tokens[1].value;
			auto				it = ctx.find(targetFunc);

			if (it == ctx.end())
			{
				std::cout << "Error: Unkown function '" << targetFunc << "'.\n";
				return (true);
			}
			if (!std::holds_alternative<Polynomial>(it->second))
			{
				std::cout << "Error: '" << targetFunc << "' is not a function/polynomial.\n";
				return (true);
			}
			Plotter plotter;
			plotter.plot(targetFunc, std::get<Polynomial>(it->second));
			return (true);
		}
		return (false);
	}
}