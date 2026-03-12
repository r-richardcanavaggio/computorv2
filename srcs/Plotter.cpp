
#include "Plotter.hpp"

Plotter::Plotter( int width, int height ) : _width(width), _height(height), _xMin(-10.0), _xMax(10.0), _yMin(-10.0), _yMax(10.0) {}

void	Plotter::plot( const std::string& funcName, const Polynomial& poly ) const
{
	std::vector<std::string>	canvas(_height, std::string(_width, ' '));

	int							originX = (0.0 - _xMin) / (_xMax - _xMin) * _width;
	int							originY = _height - 1 - ((0.0 - _yMin) / (_yMax - _yMin) * _height);

	for (int r = 0; r < _height; ++r)
		if (originX >= 0 && originX < _width) canvas[r][originX] = '|';
	for (int c = 0; c < _width; ++c)
		if (originY >= 0 && originY < _height) canvas[originY][c] = '-';
	if (originY >= 0 && originY < _height && originX >= 0 && originX < _width)
		canvas[originY][originX] = '+';

	for (int c = 0; c < _width; ++c)
	{
		double x = _xMin + (c / (double)_width) * (_xMax - _xMin);

		VarType	result = poly.eval(Real(x));
		double	y = std::get<Real>(result).getReal();

		int 	r = _height - 1 - ((y - _yMin) / (_yMax - _yMin) * _height);

		if (r >= 0 && r < _height)
			canvas[r][c] = '*';
	}
	std::cout << "Plotting " << funcName << " (" << poly.getVarName() << ") from x=[" << _xMin << ", " << _xMax << "]:\n";
	for (const auto& row : canvas)
        std::cout << row << std::endl;
}
