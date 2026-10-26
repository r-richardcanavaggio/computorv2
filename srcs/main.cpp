#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include <optional>
#include "Interpreter.hpp"


namespace
{

std::optional<std::string> readLine( std::string_view prompt )
{
	char*	input = readline(prompt.data());

	if (!input) // CTRL-D
	{
		std::cout << "\n";
		return std::nullopt;
	}

	std::string	line(input);

	if (!line.empty()) add_history(input);
	
	std::free(input);
	return line;
}

}


int	main( void )
{
	Interpreter	interpreter;

	while (interpreter.isRunning())
	{
		auto line = readLine("> ");

		if (!line) break;
		if (line->empty()) continue ;

		interpreter.processLine(*line);
	}
	return (0);
}
