
#include "Plotter.hpp"
#include "Maths.hpp"
#include <limits>

Plotter::Plotter( int width, int height ) : _width(width), _height(height), _xMin(-10.0), _xMax(10.0) {}

void	Plotter::plot( const std::string& funcName, const Polynomial& poly ) const
{
	if (_width <= 0 || _height <= 0)
		return ;

	double	xMin = _xMin;
	double	xMax = _xMax;
	double	xSpan = xMax - xMin;

	double	yMin = std::numeric_limits<double>::max();
	double	yMax = std::numeric_limits<double>::lowest();
	bool	hasValidPoints = false;

	for (int c = 0; c < _width; ++c)
	{
		double	t = (double)c / (double)(_width - 1);
		double	x = xMin + t * xSpan;
		double	y = poly.eval(Real(x)).getValue();

		if (maths::finite(y))
		{
			if (y < yMin)
				yMin = y;
			if (y > yMax)
				yMax = y;
			hasValidPoints = true;
		}
	}
	if (!hasValidPoints)
	{
		std::cout << "Cannot plot: No valid points found in this range.\n";
		return ;
	}

	double	ySpan = yMax - yMin;
	if (ySpan < 1e-9)
	{
		yMin -= 1.0;
		yMax += 1.0;
	}
	else
	{
		yMin -= ySpan * 0.1;
		yMax += ySpan * 0.1;
	}
	ySpan = yMax - yMin;

	std::vector<std::string>	canvas(_height, std::string(_width, ' '));

	int	originX = maths::round(((0.0 - xMin) / xSpan) * (_width - 1));
	int	originY = (_height - 1) - maths::round(((0.0 - yMin) / ySpan) * (_height - 1));

	for (int r = 0; r < _height; ++r)
		canvas[r][originX] = '|';
    for (int c = 0; c < _width; ++c)
		canvas[originY][c] = '-';
    if (originY >= 0 && originY < _height && originX >= 0 && originX < _width)
		canvas[originY][originX] = '+';

	for (int c = 0; c < _width; ++c)
	{
		double	t = (double)c / (double)(_width - 1);
		double	x = xMin + t * xSpan;
		double	y = poly.eval(Real(x)).getValue();

		if (!maths::finite(y))
			continue;
		int	r = (_height - 1) - maths::round(((y - yMin) / ySpan) * (_height - 1));
		if (r >= 0 && r < _height)
			canvas[r][c] = '*';
	}
	std::cout << "Plotting " << funcName
		<< "(" << poly.getVarName() << ")"
		<< " from x=[" << xMin << ", " << xMax << "]"
		<< ", y=[" << yMin << ", " << yMax << "]:\n";
	for (const auto& row : canvas)
        std::cout << row << std::endl;
}
