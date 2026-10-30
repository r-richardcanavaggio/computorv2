#pragma once

#include "RuntimeTypes.hpp"
#include <string>
#include <vector>
#include <iostream>

class Plotter
{
	private:
		Real		width;
		Real		height;
		std::vector<std::vector<bool>>	grid;

		bool	get( Real x, Real y );
	
	public:
		Plotter( Real w, Real h );
		~Plotter() = default;

		void 	set( int x, int y );
		void	drawLine( Real x0, Real y0, Real x1, Real y1 );
		void	display();
};
