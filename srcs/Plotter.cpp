
#include "Plotter.hpp"
#include "Maths.hpp"
#include <limits>
#include <SFML/Graphics.hpp>

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
		double	t = (double)c / (double)_width;
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
	if (yMin > 0.0) yMin = 0.0;
	if (yMax < 0.0) yMax = 0.0;

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

	sf::VertexArray	curve(sf::LineStrip);

	for (int c = 0; c <= _width; ++c)
	{
		double t = (double)c / (double)_width;
		double x = currentXMin + t * xSpan;
		double y = poly.eval(Real(x)).getValue();

		if (!maths::finite(y)) continue;
		double r = _height - ((y - yMin) / ySpan) * _height;

		curve.append(sf::Vertex(sf::Vector2f(c, r), sf::Color::Red));
	}

	sf::VertexArray	axes(sf::Lines);

	int				originX = ((0.0 - currentXMin) / xSpan) * _width;
	int				originY = _height - ((0.0 - yMin) / ySpan) * _height;

	if (originX >= 0 && originX <= _width)
	{
		axes.append(sf::Vertex(sf::Vector2f(originX, 0), sf::Color(150, 150, 150)));
		axes.append(sf::Vertex(sf::Vector2f(originX, _height), sf::Color(150, 150, 150)));
	}
	if (originY >= 0 && originY < _height)
	{
		axes.append(sf::Vertex(sf::Vector2f(0, originY), sf::Color(150, 150, 150)));
		axes.append(sf::Vertex(sf::Vector2f(_width, originY), sf::Color(150, 150, 150)));
	}

	int	tickSize = 5;
	int	startX = std::ceil(currentXMin);
	
    
	sf::RenderWindow	window(sf::VideoMode(800, 600), "Plotting: " + funcName);
	window.setFramerateLimit(60);

	std::cout << "Displaying graphic window for " << funcName << "(x)... Close the window to return to prompt.\n";

	while (window.isOpen())
	{
		sf::Event	event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear(sf::Color(30, 30, 30));
		window.draw(axes);
		window.draw(curve);
		window.draw(xMinText);
		window.draw(xMaxText);
		window.draw(yMinText);
		window.draw(yMaxText);
		window.display();
	}
}
