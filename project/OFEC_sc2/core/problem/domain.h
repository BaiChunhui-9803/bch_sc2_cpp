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
* this file defines domain for vector-based variable with the same value type
*
*********************************************************************************/

#ifndef OFEC_DOMAIN_H
#define OFEC_DOMAIN_H

#include "../definition.h"

namespace ofec {

	template<typename ValueType = Real>
	class Domain {
	public:
		struct SingleSet {
			bool limited = false;
			std::pair<ValueType, ValueType> limit;
		};
		Domain(size_t n = 1) :m_range(n) {}
		void set_boundary_false(int i = 0) {
			m_range[i].limited = false;
		}
		void setRange(ValueType low, ValueType upper, size_t i) {
			m_range[i].limit.first = low;
			m_range[i].limit.second = upper;
			m_range[i].limited = true;
		}
		void resize(size_t n) {
			m_range.resize(n);
		}
		const SingleSet& range(size_t i = 0)const {
			return m_range[i];
		}
		size_t size()const noexcept {
			return m_range.size();
		}
		const SingleSet& operator[](size_t i)const {
			return m_range[i];
		}
		SingleSet& operator[](size_t i) {
			return m_range[i];
		}
		void clear() {
			m_range.clear();
		}

	private:
		std::vector<SingleSet> m_range;
	};

}
#endif // !OFEC_DOMAIN_H