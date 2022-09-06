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
*-------------------------------------------------------------------------------
* class Function defines basic operations for function/numerical optimization problems
*
*********************************************************************************/

#ifndef OFEC_FUNCTION_H
#define OFEC_FUNCTION_H

#include "continuous.h"
#include "../../../utility/linear_algebra/matrix.h"
#include <algorithm>

namespace ofec {
	class Function : virtual public Continuous {
	public:
		void initialize_() override;
		void setBias(Real val);
		void setScale(Real val);
		void setGlobalOpt(Real* tran = 0);
		void setOriginalGlobalOpt(Real* opt = 0);
		void setConditionNumber(Real c);
		void setRotated(bool flag);
		void setTranlated(bool flag);
		void setScaled(bool flag);

		Real bias() const { return m_bias; }
		Real scale() const { return m_scale; }
		Real conditionNumber() const { return m_condition_number; }
		Real translation(size_t i) const { return m_translation[i]; }
		const Matrix& rotation() const { return m_rotation; }
		const std::vector<Real>& translation() const { return m_translation; }
		const Optima<>& originalOptima() const { return m_original_optima; }

		Matrix& rotation() { return m_rotation; }
		std::vector<Real>& translation() { return m_translation; }

	protected:
		void evaluateObjective(Real *x, std::vector<Real> &obj) final override;
		virtual void evaluateOriginalObj(Real *x, std::vector<Real> &obj) {}
		virtual void clear();
		virtual bool loadTranslation(const std::string& path);
		virtual void loadTranslation_(const std::string& path);
		virtual void setTranslation(const Real* opt_var);
		virtual bool loadRotation(const std::string& path);
		virtual void loadRotation_(const std::string& path);
		virtual void setRotation();

		void translateZero();
		void resizeTranslation(size_t n);
		void resizeRotation(size_t n);
		void irregularize(Real* x);
		void asyemmetricalize(Real* x, Real belta);
		void translate(Real* x);
		void translateOrigin(Real* x);
		void rotate(Real* x);
		void scale(Real* x);
		void updateParameters() override;
 
	protected:
		// translation in variable space
		std::vector<Real> m_translation;
		bool m_scaled, m_rotated, m_translated, m_noisy;
		Real m_scale, m_bias;
		Real m_condition_number;
		// dim: num_var * num_var
		Matrix m_rotation;
		Optima<> m_original_optima;
	};
	template<typename T> Problem* create_function() {
		return new T();
	}
	typedef Problem* (*pFun)();
	typedef std::vector<pFun> basic_func;
}
#endif // !OFEC_FUNCTION_H