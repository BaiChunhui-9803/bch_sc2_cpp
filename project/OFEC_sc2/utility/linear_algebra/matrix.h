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
* class Matrix generates orthogonal rotation matrixs
*
*********************************************************************************/

#ifndef OFEC_MATRIX_H
#define OFEC_MATRIX_H

#include "vector.h"

namespace ofec {
	class Matrix final {								// *****************Orthogonal rotation matrix***********************
		friend Matrix operator*(const Matrix &left_mat, const Matrix &right_mat);
		friend Matrix operator+(const Matrix &left_mat, const Matrix &right_mat);
		friend Matrix operator-(const Matrix &left_mat, const Matrix &right_mat);
		friend Matrix operator*(const Real real, const Matrix &mat);
		friend Matrix operator*(const Matrix &mat, const Real real);
	public:
		Matrix(size_t dim = 0);
		Matrix(size_t r, size_t c);
		Matrix(const Matrix &m)=default;
		Matrix(Matrix&& m);
		Matrix & operator*=(const Matrix & m);
		Matrix & operator+=(const Matrix & m);
		Matrix & operator-=(const Matrix & m);
		Matrix & operator*=(Real x);
		Matrix & operator=(const Matrix & m)=default;
		Matrix & operator=(Matrix && m);
		Vector & operator[](size_t idx);
		const Vector & operator[](size_t idx)const;

		size_t row() const { return m_row_size; }
		size_t col() const { return m_col_size; }

		bool eqaul(const Matrix & m, Real accuracy = 1e-6);
		bool identify();
		bool checkIdentity(Real accuracy = 1e-6);
		bool checkDiagonal(Real accuracy = 1e-6);
		bool checkOrthogonal(Real accuracy = 1e-6);
		bool checkInvertible(Real accuracy = 1e-6);

		void setRotationAxes(size_t r, size_t c, Real angle);
		void generateRotationModified(Normal *rand, Real CondiNum);
		void generateRotationClassical(Normal *rand, Real CondiNum);

		void randomize(Uniform * rand, Real min=-1, Real max=1);
		void randomize(Normal *rand);

		void modifiedOrthonormalize();
		void classicalOrthonormalize();
		void diagonalize(Normal * rand, Real CondiNum);
		Matrix transpose();
		void inverse();
		void diagonalize(Real alpha);
		void covarianceMatrix(Matrix &mat, Vector &mean);
		void eigendecomposition(std::vector<Real> & eigen_value, std::vector<Vector> & eigen_vector);

		void zeroize();
		void setRow(const Real *d, size_t c, size_t r = 1);
		void setCol(const Real *d, size_t r, size_t c = 1);
		void set(const std::vector<Vector> & d);
		void set(const Real * const * d);
		
		void resize(size_t row, size_t col);
		Real determinant();
		std::vector<Vector>& data();
		void load(std::ifstream &in);
		void print(std::ofstream & out);
		void clear();

	private:
		void Householder2(size_t n, std::vector<Vector> & V, std::vector<Real> & d, std::vector<Real> & e);
		void QLalgo2(size_t n, std::vector<Real> & d, std::vector<Real> & e, std::vector<Vector> & V);
	
		Real determinant_(std::vector<Vector>& temp, size_t num);
	private:
		size_t m_col_size, m_row_size;									// matrix size
		std::vector<Vector> m_row;
		bool m_det_flag = false;
		Real m_det;
	};

}

#endif
