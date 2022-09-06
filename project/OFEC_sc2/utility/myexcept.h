
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
* A simple myexcept class
*
*********************************************************************************/

#ifndef OFEC_MYEXCEPT_H
#define OFEC_MYEXCEPT_H

///
/// MyExcept class thrown by OFEC
///
namespace ofec {
	class MyExcept {
	public :
		MyExcept(const char* message)  { 
			m_message = message; 
		}
		~MyExcept() noexcept {}
		const char* what() const noexcept { return m_message; }
	private :
		const char* m_message;
	};

#define THROW(mess) throw MyExcept(mess);
}

#endif                            // end of OFEC_MYEXCEPT_H

