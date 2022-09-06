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
* class vector defines basic operations for Euclidean vector operations
*
*********************************************************************************/
#ifndef OFEC_VECTOR_H
#define OFEC_VECTOR_H

#include <vector>
#include <fstream>
#include <random>

#include "../../core/definition.h"
#include "../random/newran.h"

namespace ofec {
	class Vector final {
		friend Vector operator*(const Real val, const Vector & vec);
		friend Vector operator*(const Vector & vec, const Real val);
		friend Vector operator/(const Real val, const Vector & vec);
		friend Vector operator+(const Real val, const Vector & vec);
		friend Vector operator+(const Vector & vec, const Real val);
		friend Vector operator-(const Real val, const Vector & vec);
		friend Vector operator-(const Vector & vec, const Real val);
		friend Vector operator+(const Vector &left_vec, const Vector &righr_vec);
		friend Vector operator-(const Vector &left_vec, const Vector &righr_vec);
		friend Real operator*(const Vector &left_vec, const Vector &righr_vec);
	private:
		std::vector<Real> m_data;
		Real m_length = 0;
		bool m_wrote = true;
	public:
		class vector_ {
		private:
			Vector & m_vec;
			size_t m_idx;
		public:
			vector_(Vector& v, size_t idx);
			vector_& operator=(const vector_& rhs);
			operator Real() const;
			vector_&  operator=(Real);
			vector_& operator+=(Real);
			vector_& operator-=(Real);
			vector_& operator/=(Real);
			vector_& operator*=(Real);
		};
		friend class vector_;
		Vector(size_t n = 0);
		Vector(size_t n, Real val);
		Vector(const std::vector<Real> & v);
		Vector(const std::vector<Real> &&v);
		Vector(const Vector&) = default;
		Vector(Vector&& rhs);
		Vector& operator=(Vector&& rhs);
		const Real &operator [](size_t)const;

		std::vector<Real> & vect() { return m_data; }
		const std::vector<Real> & vect()const { return m_data; }

		Vector& operator =(const Vector & v);
		Vector& operator =(const std::vector<Real> & v);
		Vector& operator +=(const Vector & v);
		Vector& operator -=(const Vector & v);
		Vector& operator *=(Real val);
		Vector& operator /=(Real val);
		Vector& operator -=(Real val);
		Vector& operator +=(Real val);

		Vector pointBetween(const Vector & v, Real ratio)const;
		
		vector_ operator [](size_t);
		void pushBack(Real);
			
		Vector projection( const Vector &v) const;
		void normalize();

		size_t size()const;
		std::vector<Real>::iterator  begin();
		std::vector<Real>::iterator  end();

		void randomizeInSphere(Real radius, Uniform * rand);
		void randomizeOnSphere(Real radius, Uniform * rand);
		void randomizeOnSphere(Real radius, std::uniform_real_distribution<Real> &unif, std::default_random_engine &gen);
		void randomize(Uniform * rand,Real min = 0, Real max = 1);
		void randomize(std::uniform_real_distribution<Real> &unif, std::default_random_engine &gen, Real min, Real max);	
		void randomize(Normal *rand);
		void randomizeInSphere(Real radius, Normal *nor, Uniform * uni);

		Real angle( Vector & v);
		Real length();
		Real distance(const Vector &point) const;
		Real distance(const std::vector<Real> &point)const;
		void zeroize();
		void resize(size_t n) noexcept;
		Real perpendicularDistance(const Vector &point); 
	protected:
		void length_();
		friend std::ifstream &operator>>(std::ifstream &, Vector&);
	};

}

#endif
