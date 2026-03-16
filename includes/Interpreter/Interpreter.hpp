
#pragma once

#include "RuntimeTypes.hpp"
#include <vector>
#include <string>


class Interpreter
{
	private:
		Context&					_ctx;
		std::vector<std::string>	_history;

	public:
		Interpreter( Context& );
		~Interpreter();

		void	processLine( std::vector<Token>&, const std::string& );
};
