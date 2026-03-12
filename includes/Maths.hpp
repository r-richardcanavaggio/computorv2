
#pragma once

#include "Real.hpp"
#include "Types/Complex.hpp"

namespace maths
{

inline constexpr double pi = 3.14159265358979323846;

Real		min( const Real&, const Real& );
Real		max( const Real&, const Real& );

Real		abs( const Real& );
Real		abs( const Complex& );

Real		pow( const Real&, int );
Complex		pow( const Complex&, int );

Real		sqrt( const Real& );
Complex		sqrt( const Complex& );

Real		cos( const Real& x, size_t terms = 50 );
Complex		cos( const Complex& z, size_t terms = 50 );

Real		sin( const Real& r, size_t terms = 50 );
Complex		sin( const Complex& z, size_t terms = 50 );

Real		tan( const Real& x, size_t terms = 50 );
Complex		tan( const Complex& z, size_t terms = 50 );

Real		arctan( const Real& x, size_t terms = 50 );
Real		arctan2( const Real& a, const Real& b );

}