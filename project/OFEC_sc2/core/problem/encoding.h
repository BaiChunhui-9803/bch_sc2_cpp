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
* The vector-based objective_ encoding schme and a vector-based variable_ encoding scheme
* is defined.
*
*********************************************************************************/

#ifndef OFEC_ENCODING_H
#define OFEC_ENCODING_H

#include <vector>
#include <functional>
#include "../definition.h"
#include <memory>

namespace ofec {	
	class VarBase {
	public:
		virtual ~VarBase() = default;
	};
	
	class SolBase {
	protected:
		std::unique_ptr<VarBase> m_var;
		std::vector<Real> m_obj;
		std::vector<Real> m_con;
		int m_time_evaluate;
		SolBase& operator=(const SolBase& rhs);
		SolBase& operator=(SolBase&& rhs);

	public:	
	    SolBase() = default;
		SolBase(size_t num_objs, size_t num_cons);
		SolBase(const SolBase &sol);
		SolBase(SolBase &&sol);
		virtual ~SolBase() {};


		virtual void reset() { m_time_evaluate = 0; }
		size_t objectiveSize() const noexcept { return m_obj.size(); }
		size_t constraintSize() const noexcept { return m_con.size(); }
		const VarBase& variableBase() const noexcept { return *m_var; }
		const std::vector<Real>& objective() const noexcept { return m_obj; }
		const std::vector<Real>& constraint() const noexcept { return m_con; }
		Real objective(size_t idx) const noexcept { return m_obj[idx]; }
		Real constraint(size_t idx) const noexcept { return m_con[idx];; }

		Dominance compare(const SolBase &s, int id_pro) const;
		bool dominate(const std::vector<Real> &o, int id_pro) const;
		bool dominate(const SolBase &s, int id_pro) const;
		Dominance compare(const SolBase &s, const std::vector<OptMode> &mode) const;
		bool dominate(const std::vector<Real> &o, const std::vector<OptMode> &mode) const;
		bool dominate(const SolBase &s, const std::vector<OptMode> &mode) const;
		bool dominEqual(const std::vector<Real> &o, const std::vector<OptMode> &mode) const;
		bool dominEqual(const SolBase &s, const std::vector<OptMode> &mode)const;
		bool nonDominated(const std::vector<Real> &o, const std::vector<OptMode> &mode) const;
		bool nonDominated(const SolBase &s, const std::vector<OptMode> &mode) const;

		virtual void initialize(int id_pro, int id_rnd);
		template<typename Initializer, typename... Args>
		void initialize(Initializer f, Args&&... args) {
			f(std::forward<Args>(args)...);
		}

		bool equal(const SolBase &rhs, const std::vector<OptMode> &mode) const;
		bool same(const SolBase &x, int id_pro) const;
		virtual Real variableDistance(const SolBase &rhs, int id_pro) const = 0;
		virtual Real variableDistance(const VarBase &x, int id_pro) const = 0;
		Real objectiveDistance(const SolBase &rhs) const;
		Real objectiveDistance(const std::vector<Real> &rhs) const;
		bool boundaryViolated(int id_pro) const;
		bool constraintViolated(int id_pro) const;
		size_t numViolation() const;

		void resizeObjective(size_t n) { m_obj.resize(n); }
		void resizeConstraint(size_t n) { m_con.resize(n); }
		VarBase& variableBase() noexcept { return *m_var; }
		std::vector<Real>& constraint() noexcept { return m_con; }
		std::vector<Real>& objective() noexcept { return m_obj; }
		Real &objective(size_t idx) noexcept { return m_obj[idx]; }
		Real &constraint(size_t idx) noexcept { return m_con[idx]; }
		void setObjInfinite(int id_pro);

		int evaluate(int id_pro, int id_alg, bool effective_eval = true);
	};



	template <typename T>
	class VarVec:public VarBase {
	public:
		using ValueType = T;
		using Encoding = std::vector<T>;
		using IterType = typename std::vector<T>::iterator;
		using CIterType = typename std::vector<T>::const_iterator;
		explicit VarVec(size_t n=0) :m_x(n){}
		VarVec(size_t n, const T& val) : m_x(n, val) {}
		VarVec(const VarVec& rhs) = default;
		VarVec(VarVec&& rhs) noexcept = default;
		explicit VarVec(const std::vector<T>& x) : m_x(x) {}
		VarVec& operator=(const VarVec& rhs) {
			m_x = rhs.m_x;
			return *this;
		}
		VarVec& operator=(VarVec&& rhs) noexcept = default;

		void resize(size_t n) {
			m_x.resize(n);
		}
		size_t size() const noexcept {
			return m_x.size();
		}

		IterType begin() noexcept {
			return m_x.begin();
		}

		IterType end() noexcept {
			return m_x.end();
		}
		
		CIterType begin() const noexcept {
			return m_x.begin();
		}

		CIterType end() const noexcept {
			return m_x.end();
		}

		const T& operator[](size_t i)const{
			return m_x[i];
		}
		T& operator[](size_t i){
			return m_x[i];
		}
		T* data() noexcept {
			return m_x.data();
		}
		const T* data() const noexcept {
			return m_x.data();
		}
		Encoding& vect() noexcept{
			return m_x;
		}
		const Encoding& vect() const noexcept {
			return m_x;
		}
	protected:
		std::vector<T> m_x;
	};

}

#endif // !OFEC_ENCODING_H

