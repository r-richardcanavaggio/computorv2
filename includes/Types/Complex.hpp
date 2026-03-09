/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Complex.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rrichard <rrichard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/14 18:27:58 by rrichard          #+#    #+#             */
/*   Updated: 2026/03/09 21:22:04 by rrichard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include "Maths.hpp"
#include "Types/Real.hpp"

class Complex
{
	private:
		Real	real;
		Real	imag;
	
	public:
		Complex();
		Complex( Real, Real );
		Complex( Real );
		Complex( const Complex& ) = default;
		~Complex() = default;
		
		Complex		conj() const;
		Real		abs() const;
		Real		getReal() const;
		Real		getImag() const;
		void		setReal( Real );
		void		setImag( Real );

		Complex		pow( int ) const;
		
		Complex		operator+( const Complex& ) const noexcept;
		Complex&	operator+=( const Complex& ) noexcept;

		Complex		operator-( const Complex& ) const noexcept;
		Complex		operator-() const;
		Complex&	operator-=( const Complex& ) noexcept;

		Complex		operator*( const Complex& ) const noexcept;
		Complex&	operator*=( const Complex& ) noexcept;
		
		Complex		operator/( const Complex& ) const noexcept;
		Complex&	operator/=( const Complex& ) noexcept;

		Complex&	operator=( const Complex& ) = default;
		bool		operator==( const Complex& ) const noexcept;

		friend std::ostream&	operator<<( std::ostream&, const Complex& );
};

Complex	operator+( const Complex&, const Real& );
Complex	operator+( const Real&, const Complex& );
Complex	operator-( const Complex&, const Real& );
Complex	operator-( const Real&, const Complex& );
Complex	operator*( const Complex&, const Real& );
Complex	operator*( const Real&, const Complex& );
Complex	operator/( const Complex&, const Real& );
Complex	operator/( const Real&, const Complex& );