#include "matrix.h"
#include "../myexcept.h"

namespace ofec {

	Matrix::Matrix(size_t dim) :m_col_size(dim), m_row_size(dim) {
		if (m_row_size == 0) return;
		resize(m_row_size, m_col_size);

	}
	Matrix::Matrix(size_t r, size_t c) : m_col_size(c), m_row_size(r) {
		resize(m_row_size, m_col_size);
	}

	Matrix & Matrix::operator=(Matrix && m) {
		m_row = std::move(m.m_row);
		m_col_size = m.m_col_size;
		m_row_size = m.m_row_size;
		m_det_flag = m.m_det_flag;
		m_det = m.m_det;
		return *this;
	}

	Matrix::Matrix(Matrix&& m):m_row(std::move(m.m_row)), m_col_size(m.m_col_size),
		m_row_size(m.m_row_size), m_det_flag(m.m_det_flag),m_det(m.m_det){
	}

	void Matrix::setRow(const Real *d, size_t c, size_t r) {
		if (r != m_row_size) return;
		for (int i = 0; i < m_row_size; i++)
			for (int j = 0; j < m_col_size; j++)
				m_row[i][j] = d[j];
		m_det_flag = false;
	}
	void Matrix::setCol(const Real *d, size_t r, size_t c) {
		if (c != m_col_size) return;
		for (int i = 0; i < m_row_size; i++)
			for (int j = 0; j < m_col_size; j++)
				m_row[i][j] = d[i];
		m_det_flag = false;

	}
	void Matrix::set(const std::vector<Vector> & d) {
		for (int i = 0; i < m_row_size; i++)
			for (int j = 0; j < m_col_size; j++)
				m_row[i][j] = d[i][j];
		m_det_flag = false;
	}

	void Matrix::set(const Real * const * d) {
		for (int i = 0; i < m_row_size; i++)
			for (int j = 0; j < m_col_size; j++)
				m_row[i][j] = d[i][j];
		m_det_flag = false;
	}

	Matrix operator*(const Matrix &left_mat, const Matrix &right_mat) {
		//TODO: GPU computing
		if (left_mat.m_col_size != right_mat.m_row_size)
			throw MyExcept("can not *, m_col_size of 1st matrix must be equal to m_row_size of 2nd Matrix@ Matrix::operator *");
		Matrix r(left_mat.m_row_size, right_mat.m_col_size);
		for (int i = 0; i < left_mat.m_row_size; i++) {
			for (int j = 0; j < right_mat.m_col_size; j++) {
				r.m_row[i][j] = 0;
				for (int k = 0; k < left_mat.m_col_size; k++)
					r.m_row[i][j] += left_mat.m_row[i][k] * right_mat.m_row[k][j];
			}
		}
		return r;
	}

	Matrix operator+(const Matrix &left_mat, const Matrix &right_mat) {
		if (left_mat.m_row_size != right_mat.m_row_size || left_mat.m_col_size != right_mat.m_col_size)
			throw MyExcept("can not +, m_row_size and m_col_size of 1st matrix must be equal to that of 2nd matrix@ Matrix::operator +");
		Matrix r(left_mat.m_row_size, left_mat.m_col_size);
		for (int i = 0; i < left_mat.m_row_size; i++) {
			r.m_row[i] = left_mat.m_row[i] + right_mat.m_row[i];
		}
		return r;
	}

	Matrix operator-(const Matrix &left_mat, const Matrix &right_mat) {
		if (left_mat.m_row_size != right_mat.m_row_size || left_mat.m_col_size != right_mat.m_col_size)
			throw MyExcept("can not -, m_row_size and m_col_size of 1st matrix must be equal to that of 2nd matrix@ Matrix::operator -");
		Matrix r(left_mat.m_row_size, left_mat.m_col_size);
		for (int i = 0; i < left_mat.m_row_size; i++) {
			r.m_row[i] = left_mat.m_row[i] - right_mat.m_row[i];
		}
		return r;
	}

	Matrix operator*(const Real real, const Matrix &mat) {
		Matrix r(mat.m_row_size, mat.m_col_size);
		for (int i = 0; i < mat.m_row_size; i++) {
			r[i] = mat.m_row[i] * real;
		}
		return r;
	}

	Matrix operator*(const Matrix &mat, const Real real) {
		Matrix r(mat.m_row_size, mat.m_col_size);
		for (int i = 0; i < mat.m_row_size; i++) {
			r[i] = mat.m_row[i] * real;
		}
		return r;
	}

	Matrix & Matrix::operator *=(const Matrix &m) {
		//TODO: GPU computing
		if (m_col_size != m.m_row_size)
			throw MyExcept("can not *=, m_col_size of 1st matrix must be equal to m_row_size of 2nd matrix@ Matrix::operator *=");

		Matrix r(m_row_size, m.m_col_size);
		for (int i = 0; i < m_row_size; i++) {
			for (int j = 0; j < m.m_col_size; j++) {
				r.m_row[i][j] = 0;
				for (int k = 0; k < m_col_size; k++)
					r.m_row[i][j] += m_row[i][k] * m.m_row[k][j];
			}
		}
		if (m_row_size != m.m_row_size || m_col_size != m.m_col_size) {
			clear();
			resize(r.m_row_size, r.m_col_size);
		}
		copy(r.m_row.begin(), r.m_row.end(), this->m_row.begin());
		m_det_flag = false;
		return *this;
	}

	Matrix & Matrix::operator+=(const Matrix & m) {
		if (m_row_size != m.m_row_size || m_col_size != m.m_col_size)
			throw MyExcept("can not +=, m_row_size and m_col_size of 1st matrix must be equal to that of 2nd matrix@ Matrix::operator +=");
		for (int i = 0; i < m_row_size; i++) {
			m_row[i] += m.m_row[i];
		}
		return *this;
	}

	Matrix & Matrix::operator-=(const Matrix & m) {
		if (m_row_size != m.m_row_size || m_col_size != m.m_col_size)
			throw MyExcept("can not -=, m_row_size and m_col_size of 1st matrix must be equal to that of 2nd matrix@ Matrix::operator -=");
		for (int i = 0; i < m_row_size; i++) {
			m_row[i] -= m.m_row[i];
		}
		return *this;
	}

	Matrix & Matrix::operator *=(Real x) {
		for (int i = 0; i < m_row_size; i++) {
			m_row[i] *= x;
		}
		m_det_flag = false;
		return *this;
	}

	bool Matrix::eqaul(const Matrix & m, Real accuracy) {
		if ((m_col_size == m.m_col_size) && (m_row_size == m.m_row_size)) {
			for (int i = 0; i < m_row_size; i++) {
				for (int j = 0; j < m_col_size; j++) {
					if (fabs(m_row[i][j] - m.m_row[i][j]) > accuracy)
						return false;
				}
			}
			return true;
		}
		else return false;
	}

	bool Matrix::identify() {
		if (m_row_size != m_col_size) return false;
		for (int i = 0; i < m_row_size; i++)
			for (int j = 0; j < m_col_size; j++)
				m_row[i][j] = (j == i);
		m_det = 1;
		m_det_flag = true;
		return true;
	}
	bool Matrix::checkIdentity(Real accuracy) {
		if (m_row_size != m_col_size) return false;
		for (int i = 0; i < m_row_size; i++) {
			for (int j = 0; j < m_col_size; j++) {
				if (fabs(m_row[i][j] - (i == j ? 1. : 0)) > accuracy) {
					return false;
				}
			}
		}
		return true;
	}
	void Matrix::diagonalize(Normal * rand, Real CondiNum) {
		if (m_row_size != m_col_size) 
			throw MyExcept("can not be check_diagonal, matrix must be squre@Matrix::diagonalize");

		Real min, max;
		Real *r = new Real[m_row_size];
		for (int i = 0; i < m_row_size; i++)	r[i] = rand->nextNonStd(1., m_row_size*1.0);

		min = max = r[0];
		for (int i = 0; i < m_row_size; i++) {
			if (min > r[i])min = r[i];
			if (max < r[i]) max = r[i];
		}
		for (int i = 0; i < m_row_size; i++) {
			for (int j = 0; j < m_col_size; j++)
				if (j != i) m_row[i][j] = 0.;
				else m_row[i][j] = pow(CondiNum, (r[i] - min) / (max - min));
		}
		delete[]r;
		m_det_flag = false;
	}
	void Matrix::zeroize() {
		for (int i = 0; i < m_row_size; i++)
			for (int j = 0; j < m_col_size; j++)
				m_row[i][j] = 0.;
		m_det = 0;
		m_det_flag = true;
	}
	void Matrix::setRotationAxes(size_t r, size_t c, Real angle) {
		identify();
		m_row[r][r] = cos(angle);
		m_row[r][c] = -sin(angle);
		m_row[c][r] = sin(angle);
		m_row[c][c] = cos(angle);
		m_det_flag = false;
	}


	void Matrix::generateRotationModified(Normal *rand, Real CondiNum) {
		Matrix ortholeft(m_row_size), orthoright(m_row_size), check_diagonal(m_row_size);
		ortholeft.randomize(rand);
		ortholeft.modifiedOrthonormalize();
		orthoright.randomize(rand);
		orthoright.modifiedOrthonormalize();
		check_diagonal.diagonalize(rand, CondiNum);
		ortholeft *= check_diagonal;
		ortholeft *= orthoright;
		*this = ortholeft;
		m_det_flag = false;
	}

	void Matrix::generateRotationClassical(Normal *rand, Real CondiNum) {
		/*P. N. Suganthan, N. Hansen, J. J. Liang, K. Deb, Y.-P. Chen, A. Auger and S. Tiwari,
		"Problem Definitions and Evaluation Criteria for the CEC 2005 Special Session on Real-Parameter
		Optimization", Technical Report, Nanyang Technological University, Singapore, May 2005 AND KanGAL
		Report #2005005, IIT Kanpur, India.*/

		Matrix ortholeft(m_row_size), orthoright(m_row_size), check_diagonal(m_row_size);
		ortholeft.randomize(rand);
		ortholeft.classicalOrthonormalize();
		orthoright.randomize(rand);
		orthoright.classicalOrthonormalize();
		check_diagonal.diagonalize(rand, CondiNum);
		ortholeft *= check_diagonal;
		ortholeft *= orthoright;
		*this = ortholeft;
		m_det_flag = false;
	}

	void Matrix::randomize(Uniform * rand, Real min, Real max) {
		for (int i = 0; i < m_row_size; i++)
			m_row[i].randomize(rand, min, max);
		m_det_flag = false;
	}
	void Matrix::randomize(Normal *rand) {
		for (int i = 0; i < m_row_size; i++)
			m_row[i].randomize(rand);
		m_det_flag = false;
	}

	void Matrix::modifiedOrthonormalize() {
		if (m_row_size != m_col_size) 
			throw MyExcept("cannot perform orthonormalization, matrix must be squre@Matrix::orthonormalize");
		//if (!(checkInvertible())) throw MyExcept("cannot perform function orthonormalize(), matrix must be reversible@matrix::orthonormalize");
		Matrix v(m_row_size, m_col_size);
		Real r = 0;
		//modified Gram schmidt process
		for (int i = 0; i < m_row_size; i++) {
			v[i] = m_row[i];
		}
		for (int i = 0; i < m_row_size; i++) {
			v[i].normalize();
			m_row[i] = v[i];
			for (int j = i; j < m_row_size; j++) {
				r = v[j] * m_row[i];
				v[j] -= m_row[i] * r;
			}
		}
		m_det_flag = false;
	}
	bool Matrix::checkOrthogonal(Real accuracy) {
		/*if (m_row_size != m_col_size) throw MyExcept("cannot perform function checkOrthogonal(), matrix must be squre@matrix::orthonormalize");
		Matrix temp1 = *this;
		temp1.transpose();
		const Matrix& temp2 = temp1;
		temp1 *= temp2;
		return temp1.checkIdentity();*/
		if (m_row_size != m_col_size) 
			throw MyExcept("cannot perform function orthogonal(), matrix must be squre@Matrix::orthogonal");

		for (int i = 0; i < m_row_size; i++) {
			for (int j = i + 1; j < m_row_size; j++) {
				if (fabs(m_row[i] * m_row[j]) > accuracy) return false;
			}
			if (fabs(m_row[i].length() - 1.) > accuracy) return false;
		}
		return true;
	}

	Matrix Matrix::transpose() {
		Matrix r(m_col_size, m_row_size);
		for (int i = 0; i < m_row_size; i++)
			for (int j = 0; j < m_col_size; j++) {
				r[j][i] = m_row[i][j];
			}
		return r;
	}

	void Matrix::classicalOrthonormalize() {
		if (m_row_size != m_col_size) throw MyExcept("cannot perform classical_orthonormalize, matrix must be squre@Matrix::classical_orthonormalize");
		Matrix v(m_row_size, m_col_size);
		Real r = 0;
		//classical Gram schmidt process
		for (int i = 0; i < m_row_size; i++) {
			v[i] = m_row[i];
			for (int j = 0; j < i; j++) {
				r = m_row[i] * m_row[j];
				v[i] -= m_row[j] * r;
			}
			v[i].normalize();
			m_row[i] = v[i];
		}
		m_det_flag = false;
	}

	void Matrix::inverse() {
		if (m_row_size != m_col_size)
			throw MyExcept("cannot perform function inverse(), matrix must be squre@Matrix::inverse");
		if (!(checkInvertible()))
			throw MyExcept("cannot perform function inverse(), matrix must be reversible@Matrix::inverse");
		if (checkDiagonal()) {
			for (int i = 0; i < m_row_size; i++)
				if (m_row[i][i] != 0)m_row[i][i] = 1. / m_row[i][i];
		}
		else {
			std::vector<std::vector<Real>> a(m_row_size, std::vector<Real>(2 * m_col_size));

			for (int i = 0; i < m_row_size; i++) {
				std::copy(m_row[i].begin(), m_row[i].end(), a[i].begin());
			}

			for (size_t i = 0; i < m_row_size; i++) {
				for (size_t j = m_col_size; j < m_col_size * 2; j++)
					a[i][j] = (j - i == m_col_size) ? (1.0) : (0.0);
			}

			for (size_t i = 0; i < m_row_size; i++)
			{
				if (a[i][i] != 1.0)
				{
					Real tmp = a[i][i];
					a[i][i] = 1.0;
					for (size_t j = i; j < m_col_size * 2; j++)
						a[i][j] /= tmp;
				}
				for (size_t k = 0; k < m_row_size; k++)
				{
					if (i != k)
					{
						Real tmp = a[k][i];
						for (size_t j = 0; j < m_col_size * 2; j++)
							a[k][j] -= (tmp*a[i][j]);
					}
					else continue;
				}
			}

			for (size_t i = 0; i < m_row_size; i++) {
				for (size_t j = m_col_size; j < m_col_size * 2; j++)
					m_row[i][j - m_col_size] = a[i][j];
			}

		}
		m_det_flag = false;
	}

	std::vector<Vector>& Matrix::data() {
		return m_row;
	}

	bool Matrix::checkInvertible(Real accuracy) {

		return !(fabs(determinant()) <= accuracy);
	}

	Real Matrix::determinant() {
		if (m_row_size != m_col_size) throw MyExcept("cannot perform function determinant(), matrix must be squre@Matrix::determinant");
		if (m_det_flag) return m_det;
		m_det = determinant_(m_row, m_col_size);
		m_det_flag = true;

		return m_det;
	}

	Real Matrix::determinant_(std::vector<Vector>& temp, size_t num) {

		int i = 0, j = 0, c = 0; /*i，j为行与列,c为向量b的行*/
		std::vector<Vector> temp_b(m_row_size, Vector(m_col_size, 0.)); /*定义向量b并初始化*/
		int p = 0, q = 0;/*p,q为中间变量*/
		Real sum = 0;

		if (num == 1)
			return temp[0][0];
		for (i = 0; i < num; i++)/*此处大循环实现将余子式存入向量b中*/
		{
			for (c = 0; c < num - 1; c++)
			{
				if (c < i)    /*借助c判断每行的移动方法*/
					p = 0;
				else
					p = 1;

				for (j = 0; j < num - 1; j++)
				{
					temp_b[c][j] = temp[c + p][j + 1];
				}
			}
			if (i % 2 == 0)  q = 1;
			else  q = (-1);

			sum += temp[i][0] * q*determinant_(temp_b, num - 1);
		}

		return sum;

	}
	void Matrix::clear() {
		m_row.clear();
	}
	void Matrix::resize(size_t row, size_t col) {
		m_col_size = col;
		m_row_size = row;
		m_row.resize(row);
		for (int i = 0; i < row; i++)
			m_row[i].resize(col);

	}

	Vector & Matrix::operator[](size_t idx) {
		return m_row[idx];
	}

	const Vector & Matrix::operator[](size_t idx)const {
		return m_row[idx];
	}


	void Matrix::diagonalize(Real alpha) {
		for (int i = 0; i < m_row_size; i++) {
			for (int j = 0; j < m_col_size; j++)
				if (i == j) m_row[i][j] = pow(alpha, 0.5*(i / (m_col_size - 1)));
				else m_row[i][j] = 0;
		}
		m_det_flag = false;
	}

	void Matrix::covarianceMatrix(Matrix &mat, Vector &mean) {
		if (mat.row() != m_col_size || mat.col() != m_col_size)
			mat.resize(m_col_size, m_col_size);
		if (mean.size() != m_col_size) 
			mean.resize(m_col_size);
		for (size_t j = 0; j < m_col_size; ++j)	{
			mean[j] = 0;
			for (size_t i = 0; i < m_row_size; ++i) {
				mean[j] += m_row[i][j];
			}
			mean[j] /= m_row_size;
		}
		for (size_t i = 0; i < m_col_size; ++i)	{
			for (size_t j = i; j < m_col_size; ++j) {
				if (i == j) {
					mat[i][j] = 0;
					for (size_t k = 0; k < m_row_size; ++k) {
						mat[i][j] += pow((m_row[k][i] - mean[i]),2);
					}
				}
				else {
					mat[i][j] = 0;
					mat[j][i] = 0;
					for (size_t k = 0; k < m_row_size; ++k) {
						Real temp = (m_row[k][i] - mean[i])*(m_row[k][j] - mean[j]);
						mat[i][j] += temp;
						mat[j][i] += temp;
					}
				}
			}
		}
	}

	bool Matrix::checkDiagonal(Real accuracy) {
		if (m_row_size != m_col_size) throw MyExcept("cannot perform function check_diagonal(), matrix must be squre@Matrix::check_diagonal");
		for (int i = 0; i < m_row_size; i++) {
			for (int j = 0; j < m_col_size; j++) {
				if (i == j) continue;
				if (!(fabs(m_row[i][j]) < accuracy)) return false;
			}
		}
		return true;
	}

	void Matrix::eigendecomposition(std::vector<Real> & eigen_value, std::vector<Vector> & eigen_vector) {
		/*
		Calculating eigenvalues and vectors.
		Input:
		N: dimension.
		C: symmetric (1:N)xN-matrix, solely used to copy data to Q
		niter: number of maximal iterations for QL-Algorithm.
		rgtmp: N+1-dimensional vector for temporal use.
		Output:
		diag: N eigenvalues.
		Q: Columns are normalized eigenvectors.
		*/


		std::vector<Real> temp_vec(m_col_size + 1);
		for (int i = 0; i < m_row_size; i++) {
			for (int j = 0; j < m_col_size; j++) {
				eigen_vector[i][j] = m_row[i][j];
			}
		}

		Householder2(m_row_size, eigen_vector, eigen_value, temp_vec);
		QLalgo2(m_row_size, eigen_value, temp_vec, eigen_vector);

	}

	void Matrix::Householder2(size_t n, std::vector<Vector> & V, std::vector<Real> & d, std::vector<Real> & e) {
		/*
		Householder transformation of a symmetric matrix V into tridiagonal form.
		-> n             : dimension
		-> V             : symmetric nxn-matrix
		<- V             : orthogonal transformation matrix:
		tridiag matrix == V * V_in * V^t
		<- d             : checkDiagonal
		<- e[0..n-1]     : off checkDiagonal (elements 1..n-1)

		code slightly adapted from the Java JAMA package, function private tred2()

		*/

		size_t i, j, k;

		for (j = 0; j < n; j++) {
			d[j] = V[n - 1][j];
		}

		/* Householder reduction to tridiagonal form */

		for (i = n - 1; i > 0; i--) {

			/* Scale to avoid under/overflow */

			Real scale = 0.0;
			Real h = 0.0;
			for (k = 0; k < i; k++) {
				scale = scale + fabs(d[k]);
			}
			if (scale == 0.0) {
				e[i] = d[i - 1];
				for (j = 0; j < i; j++) {
					d[j] = V[i - 1][j];
					V[i][j] = 0.0;
					V[j][i] = 0.0;
				}
			}
			else {

				/* Generate Householder vector */

				Real f, g, hh;

				for (k = 0; k < i; k++) {
					d[k] /= scale;
					h += d[k] * d[k];
				}
				f = d[i - 1];
				g = sqrt(h);
				if (f > 0) {
					g = -g;
				}
				e[i] = scale * g;
				h = h - f * g;
				d[i - 1] = f - g;
				for (j = 0; j < i; j++) {
					e[j] = 0.0;
				}

				/* Apply similarity transformation to remaining columns */

				for (j = 0; j < i; j++) {
					f = d[j];
					V[j][i] = f;
					g = e[j] + V[j][j] * f;
					for (k = j + 1; k <= i - 1; k++) {
						g += V[k][j] * d[k];
						e[k] += V[k][j] * f;
					}
					e[j] = g;
				}
				f = 0.0;
				for (j = 0; j < i; j++) {
					e[j] /= h;
					f += e[j] * d[j];
				}
				hh = f / (h + h);
				for (j = 0; j < i; j++) {
					e[j] -= hh * d[j];
				}
				for (j = 0; j < i; j++) {
					f = d[j];
					g = e[j];
					for (k = j; k <= i - 1; k++) {
						V[k][j] -= (f * e[k] + g * d[k]);
					}
					d[j] = V[i - 1][j];
					V[i][j] = 0.0;
				}
			}
			d[i] = h;
		}

		/* Accumulate transformations */

		for (i = 0; i < n - 1; i++) {
			Real h;
			V[n - 1][i] = V[i][i];
			V[i][i] = 1.0;
			h = d[i + 1];
			if (h != 0.0) {
				for (k = 0; k <= i; k++) {
					d[k] = V[k][i + 1] / h;
				}
				for (j = 0; j <= i; j++) {
					Real g = 0.0;
					for (k = 0; k <= i; k++) {
						g += V[k][i + 1] * V[k][j];
					}
					for (k = 0; k <= i; k++) {
						V[k][j] -= g * d[k];
					}
				}
			}
			for (k = 0; k <= i; k++) {
				V[k][i + 1] = 0.0;
			}
		}
		for (j = 0; j < n; j++) {
			d[j] = V[n - 1][j];
			V[n - 1][j] = 0.0;
		}
		V[n - 1][n - 1] = 1.0;
		e[0] = 0.0;

	}

	void Matrix::QLalgo2(size_t n, std::vector<Real> & d, std::vector<Real> & e, std::vector<Vector> & V) {
		/*
		-> n     : Dimension.
		-> d     : Diagonale of tridiagonal matrix.
		-> e[1..n-1] : off-checkDiagonal, output from Householder
		-> V     : Matrix output von Householder
		<- d     : eigenvalues
		<- e     : garbage?
		<- V     : basis of eigenvectors, according to d

		Symmetric tridiagonal QL algorithm, iterative
		Computes the eigensystem from a tridiagonal matrix in roughtly 3N^3 operations

		code adapted from Java JAMA package, function tql2.
		*/

		auto myhypot = [](Real a, Real b) {
			//numerically stable sqrt(a^2+b^2)
			Real r = 0;
			if (fabs(a) > fabs(b)) {
				r = b / a;
				r = fabs(a)*sqrt(1 + r*r);
			}
			else if (b != 0) {
				r = a / b;
				r = fabs(b)*sqrt(1 + r*r);
			}
			return r;
		};

		int i, k, l, m;
		Real f = 0.0;
		Real tst1 = 0.0;
		Real eps = 2.22e-16; /* Math.pow(2.0,-52.0);  == 2.22e-16 */

							   /* shift input e */
		for (i = 1; i < n; i++) {
			e[i - 1] = e[i];
		}
		e[n - 1] = 0.0; /* never changed again */

		for (l = 0; l < n; l++) {

			/* Find small subdiagonal element */

			if (tst1 < fabs(d[l]) + fabs(e[l]))
				tst1 = fabs(d[l]) + fabs(e[l]);
			m = l;
			while (m < n) {
				if (fabs(e[m]) <= eps*tst1) {
					/* if (fabs(e[m]) + fabs(d[m]+d[m+1]) == fabs(d[m]+d[m+1])) { */
					break;
				}
				m++;
			}

			/* If m == l, d[l] is an eigenvalue, */
			/* otherwise, iterate. */

			if (m > l) {  /* TODO: check the case m == n, should be rejected here!? */
				int iter = 0;
				do { /* while (fabs(e[l]) > eps*tst1); */
					Real dl1, h;
					Real g = d[l];
					Real p = (d[l + 1] - g) / (2.0 * e[l]);
					Real r = myhypot(p, 1.);

					iter = iter + 1;  /* Could check iteration count here */

									  /* Compute implicit shift */

					if (p < 0) {
						r = -r;
					}
					d[l] = e[l] / (p + r);
					d[l + 1] = e[l] * (p + r);
					dl1 = d[l + 1];
					h = g - d[l];
					for (i = l + 2; i < n; i++) {
						d[i] -= h;
					}
					f = f + h;

					/* Implicit QL transformation. */

					p = d[m];
					{
						Real c = 1.0;
						Real c2 = c;
						Real c3 = c;
						Real el1 = e[l + 1];
						Real s = 0.0;
						Real s2 = 0.0;
						for (i = m - 1; i >= l; i--) {
							c3 = c2;
							c2 = c;
							s2 = s;
							g = c * e[i];
							h = c * p;
							r = myhypot(p, e[i]);
							e[i + 1] = s * r;
							s = e[i] / r;
							c = p / r;
							p = c * d[i] - s * g;
							d[i + 1] = h + s * (c * g + s * d[i]);

							/* Accumulate transformation. */

							for (k = 0; k < n; k++) {
								h = V[k][i + 1];
								V[k][i + 1] = s * V[k][i] + c * h;
								V[k][i] = c * V[k][i] - s * h;
							}
						}
						p = -s * s2 * c3 * el1 * e[l] / dl1;
						e[l] = s * p;
						d[l] = c * p;
					}

					/* Check for convergence. */

				} while (fabs(e[l]) > eps*tst1);
			}
			d[l] = d[l] + f;
			e[l] = 0.0;
		}

		/* Sort eigenvalues and corresponding vectors. */
#if 1
		/* TODO: really needed here? So far not, but practical and only O(n^2) */
		//{
		//	int j;
		//	Real p;
		//	for (i = 0; i < n - 1; i++) {
		//		k = i;
		//		p = d[i];
		//		for (j = i + 1; j < n; j++) {
		//			if (d[j] < p) {
		//				k = j;
		//				p = d[j];
		//			}
		//		}
		//		if (k != i) {
		//			d[k] = d[i];
		//			d[i] = p;
		//			for (j = 0; j < n; j++) {
		//				p = V[j][i];
		//				V[j][i] = V[j][k];
		//				V[j][k] = p;
		//			}
		//		}
		//	}
		//}
#endif 
	}


	void Matrix::load(std::ifstream &in) {
		for (int i = 0; i < m_row_size; i++) {
			in >> m_row[i];
		}
	};
	void Matrix::print(std::ofstream & out) {
		for (int i = 0; i < m_row_size; i++) {
			for (int j = 0; j < m_col_size; j++)
				out << m_row[i][j] << " ";
			out << std::endl;
		};
	}

}