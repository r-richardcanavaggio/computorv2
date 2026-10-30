#pragma once

#include <map>
#include <string>
#include <variant>

#include "Types/Complex.hpp"
#include "Types/Matrix.hpp"
#include "Types/Real.hpp"
#include "Types/Token.hpp"
#include "Types/Polynomial.hpp"

using VarType = std::variant<Real, Complex, Polynomial, Matrix<Real>, Matrix<Complex>>;

struct Context : std::map<std::string, VarType>
{
	bool	isDegreeMode = false;
};

inline std::ostream&	operator<<( std::ostream& os, const VarType& v )
{
	std::visit([&os]( const auto& value ) { os << value; }, v);
	return (os);
}
