//Register JADE "JADE" GOP,ConOP,SOP,MMOP

/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*********************************************************************************/
// updated Mar 28, 2018 by Li Zhou
// Updated on 15th Aug, 2019 by Junchen Wang

/*
Paper: J. Zhang and C. Arthur, JADE: Adaptive differential evolution with
optional external archive, IEEE Trans. Evol. Comput., vol. 13, no. 5,
pp. 945-958, Oct. 2009.
*/

#ifndef OFEC_JADE_H
#define OFEC_JADE_H

#include "jade_pop.h"
#include "../../../../core/algorithm/algorithm.h"

namespace ofec {
	class JADE : public Algorithm {
	protected:
		std::unique_ptr<PopJADE> m_pop;
		size_t m_pop_size;

		void initialize_() override;
		void run_() override;
#ifdef OFEC_DEMO
		void updateBuffer();
#endif

	public:
		void record() override;
	};
}
#endif // OFEC_JADE_H
