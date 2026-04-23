
#include "Plotter.hpp"
#include "Maths.hpp"
#include <limits>

Plotter::Plotter( int width, int height ) : _width(width), _height(height), _xMin(-10.0), _xMax(10.0) {}

void	Plotter::plot( const std::string& funcName, const Polynomial& poly ) const
{
	if (_width <= 0 || _height <= 0)
		return ;

	double	currentXMin = _xMin;
	double	currentXMax = _xMax;

	int	degree = poly.degree();

	if (degree == 0)
	{
		currentXMin = -5.0;
		currentXMax = 5.0;
	}
	else if (degree == 1)
	{
		auto coeffs = poly.getCoeffs();
		double a = coeffs[1].getValue();
		double b = coeffs[0].getValue();
		
		if (a != 0.0)
		{
			double root = -b / a;
			currentXMin = root - 10.0;
			currentXMax = root + 10.0;
		}
	}
	else if (degree == 2)
	{
		auto 	coeffs = poly.getCoeffs();
		double	a = coeffs[2].getValue();
		double	b = coeffs[1].getValue();

		if (a != 0.0)
		{
			double vertexX = -b / (2.0 * a);
			currentXMin = vertexX - 10.0;
			currentXMax = vertexX + 10.0;
		}
	}

	double	xSpan = currentXMax - currentXMin;

	double	yMin = std::numeric_limits<double>::max();
	double	yMax = std::numeric_limits<double>::lowest();
	bool	hasValidPoints = false;

	for (int c = 0; c < _width; ++c)
	{
		double	t = (double)c / (double)(_width - 1);
		double	x = currentXMin + t * xSpan;
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

	int	originX = maths::round(((0.0 - currentXMin) / xSpan) * (_width - 1));
	int	originY = (_height - 1) - maths::round(((0.0 - yMin) / ySpan) * (_height - 1));

	if (originX >= 0 && originX < _width)
	{
		for (int r = 0; r < _height; ++r)
			canvas[r][originX] = '|';
	}
	if (originY >= 0 && originY < _height)
	{
		for (int c = 0; c < _width; ++c)
			canvas[originY][c] = '-';
	}
    if (originY >= 0 && originY < _height && originX >= 0 && originX < _width)
		canvas[originY][originX] = '+';

	for (int c = 0; c < _width; ++c)
	{
		double	t = (double)c / (double)(_width - 1);
		double	x = currentXMin + t * xSpan;
		double	y = poly.eval(Real(x)).getValue();

		if (!maths::finite(y))
			continue;
		int	r = (_height - 1) - maths::round(((y - yMin) / ySpan) * (_height - 1));
		if (r >= 0 && r < _height)
			canvas[r][c] = '*';
	}
	std::cout << "Plotting " << funcName
		<< "(" << poly.getVarName() << ")"
		<< " from x=[" << currentXMin << ", " << currentXMin << "]"
		<< ", y=[" << yMin << ", " << yMax << "]:\n";
	for (const auto& row : canvas)
        std::cout << row << std::endl;
}
