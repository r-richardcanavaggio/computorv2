#include "Matrix.hpp"

template<real_complex K>
void	Matrix<K>::scl( const Real& scalar )
{
	for (size_t i = 0; i < getSize(); i++)
		operator[](i) *= scalar;
}

template<real_complex K>
Matrix<K>	Matrix<K>::operator*( const Matrix& other ) const
{
	if (getCols() != other.getCols() || getRows() != other.getRows())
		throw std::runtime_error("Error: dimensions must be identical for element-wise multiplication");

	Matrix	result(getRows(), getCols());
	
	for (std::size_t r = 0; r < getRows(); r++)
		for (std::size_t c = 0; c < getCols(); c++)
			result(r, c) = at(r, c) * other(r, c);
	return (result);
}

template<real_complex K>
Matrix<K>	Matrix<K>::operator*( const Real& scalar ) const
{
	Matrix	m = *this;

	m.scl(scalar);
	return (m);
}

template<real_complex K>
Matrix<K>	Matrix<K>::dot( const Matrix<K>& other ) const
{
	if (getCols() != other.getRows())
		throw std::runtime_error("Error: wrong dimensions for matrix multiplication");
		
	Matrix<K>	result(getRows(), other.getCols());

	for (std::size_t r = 0; r < getRows(); r++)
	{
		for (std::size_t c = 0; c < other.getCols(); c++)
		{
			K sum = K(0);

			for (std::size_t k = 0; k < other.getRows(); k++)
					sum += at(r, k) * other(k, c);
			result(r, c) = sum;
		}
	}
	return (result);
}

template<real_complex K>
Matrix<K>	Matrix<K>::pow( int exp ) const
{
	if (getRows() != getCols())
		throw std::runtime_error("Power only for square matrices");
	if (exp < 0)
		throw std::runtime_error("Negative matrix power not supported");

	Matrix<K>	res = identity();
	Matrix<K>	base = *this;

	while (exp > 0)
	{
		if (exp % 2 == 1)
			res = res.dot(base);
		base = base.dot(base);
		exp /= 2;
	}
	return (res);
}

template class Matrix<Real>;
template class Matrix<Complex>;
