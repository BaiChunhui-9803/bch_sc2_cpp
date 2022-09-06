//Register BBOB_F01 "BBOB_F01" GOP,ConOP,SOP
//Register BBOB_F02 "BBOB_F02" GOP,ConOP,SOP
//Register BBOB_F03 "BBOB_F03" GOP,ConOP,SOP
//Register BBOB_F04 "BBOB_F04" GOP,ConOP,SOP
//Register BBOB_F05 "BBOB_F05" GOP,ConOP,SOP
//Register BBOB_F06 "BBOB_F06" GOP,ConOP,SOP
//Register BBOB_F07 "BBOB_F07" GOP,ConOP,SOP
//Register BBOB_F08 "BBOB_F08" GOP,ConOP,SOP
//Register BBOB_F09 "BBOB_F09" GOP,ConOP,SOP
//Register BBOB_F10 "BBOB_F10" GOP,ConOP,SOP
//Register BBOB_F11 "BBOB_F11" GOP,ConOP,SOP
//Register BBOB_F12 "BBOB_F12" GOP,ConOP,SOP
//Register BBOB_F13 "BBOB_F13" GOP,ConOP,SOP
//Register BBOB_F14 "BBOB_F14" GOP,ConOP,SOP
//Register BBOB_F15 "BBOB_F15" GOP,ConOP,SOP
//Register BBOB_F16 "BBOB_F16" GOP,ConOP,SOP
//Register BBOB_F17 "BBOB_F17" GOP,ConOP,SOP
//Register BBOB_F18 "BBOB_F18" GOP,ConOP,SOP
//Register BBOB_F19 "BBOB_F19" GOP,ConOP,SOP
//Register BBOB_F20 "BBOB_F20" GOP,ConOP,SOP
//Register BBOB_F21 "BBOB_F21" GOP,ConOP,SOP
//Register BBOB_F22 "BBOB_F22" GOP,ConOP,SOP
//Register BBOB_F23 "BBOB_F23" GOP,ConOP,SOP
//Register BBOB_F24 "BBOB_F24" GOP,ConOP,SOP

/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// source code refered to bbob.v15.02, http://coco.gforge.inria.fr/doku.php?id=bbob-2015-downloads
#ifndef OFEC_BBOB_H
#define OFEC_BBOB_H

#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../utility/linear_algebra/matrix.h"
#include "../metrics_gop.h"
#include <string>
#include <algorithm>

namespace ofec {
	class BBOB final : public MetricsGOP {
	protected:
		void initialize_() override;
		void compute_Xopt();
		Real compute_Fopt();
		Real penalize(Real *x);
		bool loadRotation(Real base_);
		void compute_rotation(Matrix& rot, size_t Dim);
		void reshape(Matrix &B, std::vector<Real>& vector, size_t m, size_t n);
		void irregularize(Real *x);
		void asyemmetricalize(Real *x, Real belta);
		void evaluateObjective(Real *x, std::vector<Real>& obj) override;

	private:
		void f1_sphere(Real *x, std::vector<Real>& obj);
		void f2_ellipsoidale(Real *x, std::vector<Real>& obj);
		void f3_rastrigin(Real *x, std::vector<Real>& obj);
		void f4_buche_rastrigin(Real *x, std::vector<Real>& obj);
		void f5_slope(Real *x, std::vector<Real>& obj);
		void f6_sector(Real *x, std::vector<Real>& obj);
		void f7_step_ellipsoid(Real *x, std::vector<Real>& obj);
		void f8_original_rosenbrock(Real *x, std::vector<Real>& obj);
		void f9_rotated_rosenbrock(Real *x, std::vector<Real>& obj);
		void f10_nonseparable_ellipsoid(Real *x, std::vector<Real>& obj);
		void f11_discus(Real *x, std::vector<Real>& obj);
		void f12_bent_cigar(Real *x, std::vector<Real>& obj);
		void f13_sharp_ridge(Real *x, std::vector<Real>& obj);
		void f14_different_powers(Real *x, std::vector<Real>& obj);
		void f15_nonseparable_rastrigin(Real *x, std::vector<Real>& obj);
		void f16_weierstrass(Real *x, std::vector<Real>& obj);
		void f17_schaffers_F7(Real *x, std::vector<Real>& obj);
		void f18_illconditioned_schaffers_F7(Real *x, std::vector<Real>& obj);
		void f19_composite_griewank_rosenbrock(Real *x, std::vector<Real>& obj);
		void f20_schwefel(Real *x, std::vector<Real>& obj);
		void f21_gallagher_gaussian101me_peaks(Real *x, std::vector<Real>& obj);
		void f22_gallagher_gaussian21hi_peaks(Real *x, std::vector<Real>& obj);
		void f23_katsuura(Real *x, std::vector<Real>& obj);
		void f24_lunacekbi_rastrigin(Real *x, std::vector<Real>& obj);

	private:
		typedef void (BBOB::*function_ptr)(Real *x, std::vector<Real>& obj);
		Real m_beta, m_alpha;
		function_ptr m_fun = nullptr;
		Matrix m_rot, m_rot2, m_linearTF;
		std::vector<Real> m_norRand;
		Real m_mu, m_scales, m_bias;
		Real m_condition_number;
	
		//for f21 and f22
		std::vector<Real> m_peak_values;
		std::vector < std::vector < Real >> m_arrScales;
		std::vector < std::vector < Real >> m_Xlocal;
		//for F16
		std::vector<Real> m_aK, m_bK;
		Real m_F0;
	};

	int compare_doubles(const void *a, const void *b);

	using BBOB_F01 = BBOB;
	using BBOB_F02 = BBOB;
	using BBOB_F03 = BBOB;
	using BBOB_F04 = BBOB;
	using BBOB_F05 = BBOB;
	using BBOB_F06 = BBOB;
	using BBOB_F07 = BBOB;
	using BBOB_F08 = BBOB;
	using BBOB_F09 = BBOB;
	using BBOB_F10 = BBOB;
	using BBOB_F11 = BBOB;
	using BBOB_F12 = BBOB;
	using BBOB_F13 = BBOB;
	using BBOB_F14 = BBOB;
	using BBOB_F15 = BBOB;
	using BBOB_F16 = BBOB;
	using BBOB_F17 = BBOB;
	using BBOB_F18 = BBOB;
	using BBOB_F19 = BBOB;
	using BBOB_F20 = BBOB;
	using BBOB_F21 = BBOB;
	using BBOB_F22 = BBOB;
	using BBOB_F23 = BBOB;
	using BBOB_F24 = BBOB;
}
#endif // !OFEC_BBOB_H
