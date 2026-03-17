
#pragma once

#include "RuntimeTypes.hpp"
#include <string>
#include <vector>
#include <iostream>

class Plotter
{
	private:
		int		_width;
		int		_height;
		double	_xMin;
		double	_xMax;
	
	public:
		Plotter( int width = 80, int height = 40 );
		~Plotter() = default;

		void	plot( const std::string&, const Polynomial& ) const;
};