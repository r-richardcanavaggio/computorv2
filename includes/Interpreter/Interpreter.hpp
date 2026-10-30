#pragma once

#include "RuntimeTypes.hpp"
#include <vector>
#include <string>
#include <map>
#include <span>
#include <functional>

using CommandHandler = std::function<void(std::span<const Token>)>;

class Interpreter
{
	private:
		Context									ctx;
		std::vector<std::string>				history;
		std::map<std::string, CommandHandler>	commands;

		bool									running;

		bool	executeCommand( const std::vector<Token>& );
		void	commandList( std::span<const Token> ) const;
		void	commandClear( std::span<const Token> );
		void	commandHistory( std::span<const Token> ) const;
		void	commandDeg( std::span<const Token> );
		void	commandRad( std::span<const Token> );
		void	commandExit( std::span<const Token> );
		void	commandHelp( std::span<const Token> ) const;
		void	commandPlot( std::span<const Token> );

	public:
		Interpreter();

		bool	isRunning() const;
		void	processLine( const std::string& );
};
