#pragma once

#include "RuntimeTypes.hpp"
#include <vector>
#include <string>
#include <map>
#include <span>
#include <functional>

using CommandHandler = std::function< void ( const Token& ) >;

class Interpreter
{
	private:
		Context												ctx;
		std::vector<std::string>							history;
		std::map<std::string, CommandHandler, std::less<>>	commands;

		bool												running;

		bool	executeCommand( std::span<const Token> );
		void	commandList( const Token& ) const;
		void	commandClear( const Token& );
		void	commandHistory( const Token& ) const;
		void	commandDeg( const Token& );
		void	commandRad( const Token& );
		void	commandExit( const Token& );
		void	commandHelp( const Token& ) const;

	public:
		Interpreter();

		bool	isRunning() const;
		void	processLine( const std::string& );
};
