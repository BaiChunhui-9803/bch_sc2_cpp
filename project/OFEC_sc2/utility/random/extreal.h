/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* An extended real type 
*
*
*********************************************************************************/
#ifndef EXTREAL_LIB_H
#define EXTREAL_LIB_H 0

#include <iostream>
#include "../../core/definition.h"

namespace ofec {
	/************************ extended real class ***************************/
	enum class ExtRealCode
	{
		Finite, PlusInfinity, MinusInfinity, Indefinite, Missing
	};

	class ExtReal
	{
		Real m_value;
		ExtRealCode m_c;
	public:
		ExtReal operator+(const ExtReal&) const;
		ExtReal operator-(const ExtReal&) const;
		ExtReal operator*(const ExtReal&) const;
		ExtReal operator-() const;
		friend std::ostream& operator<<(std::ostream&, const ExtReal&);
		ExtReal(Real v) : m_value(v), m_c(ExtRealCode::Finite) {   }
		ExtReal(const ExtRealCode& cx) :m_c(cx) { }
		ExtReal() :m_c(ExtRealCode::Missing) {  }
		Real value() const { return m_value; }
		bool isReal() const { return m_c == ExtRealCode::Finite; }
		ExtRealCode code() const { return m_c; }
	};
}

#endif

// body file: extreal.cpp



