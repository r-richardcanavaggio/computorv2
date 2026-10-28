#include "Matrix.hpp"

template<real_complex K>
void	Matrix<K>::add( const Matrix<K>& other )
{
	if (getRows() != other.getRows() || getCols() != other.getCols())
		throw std::runtime_error("Error: Matrices must have the same dimensions for addition");
	for (std::size_t i = 0; i < getSize(); i++)
		operator[](i) += other[i];
}

template<real_complex K>
Matrix<K>	Matrix<K>::operator+( const Matrix<K>& other ) const
{
	Matrix	m = *this;

	m.add(other);
	return (m);
}

template<real_complex K>
void	Matrix<K>::sub( const Matrix<K>& other )
{
	if (getRows() != other.getRows() || getCols() != other.getCols())
		throw std::runtime_error("Error: Matrices must have the same dimensions for substraction");
	for (size_t i = 0; i < getSize(); i++)
		operator[](i) -= other[i];
}

template<real_complex K>
Matrix<K>	Matrix<K>::operator-( const Matrix<K>& other ) const
{
	Matrix	m = *this;

	m.sub(other);
	return (m);
}

template<real_complex K>
Matrix<K>	Matrix<K>::operator-() const
{
	Matrix m = *this;

	m.scl(-1.);
	return (m);
}

template class Matrix<Real>;
template class Matrix<Complex>;
