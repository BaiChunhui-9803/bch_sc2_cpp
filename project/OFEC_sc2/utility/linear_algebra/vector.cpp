#include "vector.h"
#include "../myexcept.h"
#include <algorithm>

namespace ofec {

	Vector::vector_::vector_(Vector& v, size_t idx) :m_vec(v), m_idx(idx) {

	}
	Vector::vector_& Vector::vector_::operator=(const vector_& rhs) {
		m_vec.m_data[m_idx] = rhs.m_vec.m_data[rhs.m_idx];
		m_vec.m_wrote = true;
		return *this;
	}
	Vector::vector_::operator Real()const {
		return m_vec.m_data[m_idx];
	}
	Vector::vector_&  Vector::vector_::operator=(Real val) {
		m_vec.m_data[m_idx] = val;
		m_vec.m_wrote = true;
		return *this;
	}

	Vector::vector_ & Vector::vector_::operator+=(Real val) {
		m_vec.m_data[m_idx] += val;
		m_vec.m_wrote = true;
		return *this;
	}

	Vector::vector_ & Vector::vector_::operator-=(Real val) {
		m_vec.m_data[m_idx] -= val;
		m_vec.m_wrote = true;
		return *this;
	}

	Vector::vector_ & Vector::vector_::operator*=(Real val) {
		m_vec.m_data[m_idx] *= val;
		m_vec.m_wrote = true;
		return *this;

	}
	Vector::vector_ & Vector::vector_::operator/=(Real val) {
		m_vec.m_data[m_idx] /= val;
		m_vec.m_wrote = true;
		return *this;
	}

	Vector::Vector(size_t size) : m_data(size) {

	}
	Vector::Vector(size_t size, Real val) : m_data(size, val){

	}
	Vector::Vector(const std::vector<Real> & v) : m_data(v) {

	}
	Vector::Vector(const std::vector<Real> &&v) : m_data(std::move(v)) {

	}

	Vector::vector_ Vector::operator [](size_t idx) {
		return Vector::vector_(*this, idx);
	}
	const Real & Vector::operator [](size_t idx)const {
		return m_data[idx];
	}
	Vector &Vector::operator =(const Vector & v) {
		if (this == &v) return *this;
		m_data = v.m_data;
		m_length = v.m_length;
		m_wrote = v.m_wrote;
		return *this;
	}
	Vector &  Vector::operator +=(const Vector & v) {
		if (m_data.size() != v.m_data.size()) throw MyExcept("the size of two vectors must be same by + operation@Vector::operator +=");
		std::transform(m_data.begin(), m_data.end(), v.m_data.begin(), m_data.begin(), std::plus<Real>());
		m_wrote = true;
		return *this;
	}
	Vector &  Vector::operator -=(const Vector & v) {
		if (m_data.size() != v.m_data.size()) throw MyExcept("the size of two vectors must be same by - operation@Vector::operator -=");
		std::transform(m_data.begin(), m_data.end(), v.m_data.begin(), m_data.begin(), std::minus<Real>());
		m_wrote = true;
		return *this;
	}
	Real operator*(const Vector &left_vec, const Vector &righr_vec) {
		if (left_vec.m_data.size() != righr_vec.m_data.size()) throw MyExcept("the size of two vectors must be same by * operation@Vector::operator *");
		Real sum = 0;
		for (unsigned i = 0; i < left_vec.m_data.size(); i++) sum += left_vec.m_data[i] * righr_vec.m_data[i];
		return sum;
	}

	Vector Vector::projection( const Vector &v) const{
		Vector r(v);
		Real sv = 0;
		for (unsigned i = 0; i < m_data.size(); i++)
			sv += m_data[i] * v[i];

		sv/= v*v;
		r *= sv;
		return r;
	}

	void Vector::normalize() {
		if (length() == 0) throw MyExcept("std::vector length is zero @ Vector::normalize()");
		for (auto &i : m_data) {
			i /= length();
		}
		m_length = 1;
		m_wrote = false;
	}

	size_t Vector::size()const {
		return m_data.size();
	}
	void Vector::randomizeInSphere(Real radius, Uniform * rand) {

		Real r = rand->nextNonStd<Real>(0., radius);
		randomize(rand,-1, 1);
		normalize();
		for (auto&i : m_data) i *= r;
		m_wrote = true;

	}
	void Vector::randomizeOnSphere(Real radius, Uniform * rand) {

		randomize(rand,-1, 1);
		normalize();
		for (auto&i : m_data) i *= radius;
		m_length = radius;
		m_wrote = false;

	}

	void Vector::randomizeOnSphere(Real radius, std::uniform_real_distribution<Real> &unif, std::default_random_engine &gen) {

		randomize(unif, gen, -1, 1);
		normalize();
		for (auto&i : m_data) i *= radius;
		m_length = radius;
		m_wrote = false;

	}
	void Vector::randomize(std::uniform_real_distribution<Real> &unif, std::default_random_engine &gen, Real min, Real max) {
		for (auto&i : m_data) i = min + (unif(gen) - unif.min()) / (unif.max() - unif.min())*(max - min);
		m_wrote = true;
	}
	void Vector::randomize(Uniform * rand,Real min, Real max) {
		for (auto&i : m_data) i = rand->nextNonStd(min, max);
		m_wrote = true;
	}
	void Vector::randomize(Normal *rand) {

		for (auto&i : m_data) i = rand->next();
		m_wrote = true;
	}
	void Vector::randomizeInSphere(Real radius, Normal *nor, Uniform *uni) {

		Real r = fabs(nor->nextNonStd(0, radius));
		randomize(uni,-1, 1);
		normalize();
		for (auto&i : m_data) i *= r;
		m_wrote = true;

	}
	Vector &Vector::operator =(const std::vector<Real> & v) {
		if (m_data.size() == v.size()) {
			m_data = v;
			m_wrote = true;
		}
		else {
			throw MyExcept("size not the same @ Vector::operator =(std::vector<Real>&)");
		}
		return *this;
	}

	Vector & Vector::operator *=(Real val) {
		for (auto &i : m_data) i *= val;
		m_wrote = true;
		return *this;
	}
	Vector & Vector::operator /=(Real val) {
		for (auto &i : m_data) i /= val;
		m_wrote = true;
		return *this;
	}
	Vector & Vector::operator -=(Real val) {
		for (auto &i : m_data) i -= val;
		m_wrote = true;
		return *this;
	}
	Vector & Vector::operator +=(Real val) {
		for (auto &i : m_data) i += val;
		m_wrote = true;
		return *this;
	}
	std::vector<Real>::iterator  Vector::begin() {
		return m_data.begin();
	}
	std::vector<Real>::iterator  Vector::end() {
		return m_data.end();
	}

	Vector operator*(const Real val, const Vector & vec) {
		std::vector<Real> v(vec.m_data);
		for (auto&i : v) i *= val;
		return Vector(v);
	}
	Vector operator*(const Vector & vec, const Real val) {
		std::vector<Real> v(vec.m_data);
		for (auto&i : v) i *= val;
		return Vector(v);
	}

	Vector operator/(const Real val, const Vector & vec) {
		std::vector<Real> v(vec.m_data);
		for (auto&i : v) i /= val;
		return Vector(v);
	}

	Vector operator+(const Real val, const Vector & vec) {
		std::vector<Real> v(vec.m_data);
		for (auto&i : v) i += val;
		return Vector(v);
	}
	Vector operator+(const Vector & vec, const Real val) {
		std::vector<Real> v(vec.m_data);
		for (auto&i : v) i += val;
		return Vector(v);
	}

	Vector operator-(const Real val, const Vector & vec) {
		std::vector<Real> v(vec.m_data);
		for (auto&i : v) i -= val;
		return Vector(v);
	}
	Vector operator-(const Vector & vec, const Real val) {
		std::vector<Real> v(vec.m_data);
		for (auto&i : v) i -= val;
		return Vector(v);
	}

	Vector operator+(const Vector &left_vec, const Vector &righr_vec) {
		if (left_vec.m_data.size() != righr_vec.m_data.size()) throw MyExcept("the size of two vectors must be same by + operation@Vector::operator +");
		Vector r(left_vec);
		return r += righr_vec;
	}

	Vector operator-(const Vector &left_vec, const Vector &righr_vec) {
		if (left_vec.m_data.size() != righr_vec.m_data.size()) throw MyExcept("the size of two vectors must be same by - operation@Vector::operator -");
		Vector r(left_vec);
		return r -= righr_vec;
	}

	Real Vector::angle( Vector & v){		
		return acos((*this)*(v) / (length()*v.length()));
	}

	void Vector::pushBack(Real n) {
		m_data.push_back(n);
		m_wrote = true;
	}

	Real Vector::distance(const Vector& point) const {
		if (point.size() != size()) throw MyExcept("size not the same@  Vector::distance(const Vector&v)");
		Vector vec(m_data);
		vec -= point;
		return vec.length();
	}
	Real Vector::distance(const std::vector<Real>& point) const {
		if (point.size() != size()) throw MyExcept("size not the same@  Vector::distance(const Vector&v)");
		Vector vec(m_data);
		vec -= point;
		return vec.length();
	}

	std::ifstream &operator>>(std::ifstream & in, Vector&v) {
		for (auto &i : v.m_data) {
			in >> i;
		}
		v.m_wrote = true;
		return in;
	}
	Real Vector::length() {
		if (m_wrote) {
			length_();
			m_wrote = false;
		}
		return m_length;
	}
	Vector::Vector(Vector&& rhs) :m_data(move(rhs.m_data)), m_length(rhs.m_length), m_wrote(rhs.m_wrote) {

	}

	Vector& Vector::operator=(Vector&& rhs) {
		m_data = std::move(rhs.m_data);
		m_length = rhs.m_length;
		m_wrote = rhs.m_wrote;

		return *this;
	}

	void Vector::zeroize() {
		for (auto &i : m_data) 		i = 0;
		m_length = 0;
		m_wrote = false;
	}

	Vector Vector::pointBetween(const Vector & v1, Real r) const{
		std::vector<Real> v(m_data);
		for (auto i = 0; i < m_data.size(); ++i) {
			v[i] = m_data[i] * r + v1[i] * (1 - r);
		}
		return Vector(v);
	}

	void  Vector::resize(size_t n) noexcept {
		m_data.resize(n);
		m_wrote = true;
	}

	void Vector::length_() {
		m_length = 0;
		for (auto &i : m_data) m_length += i*i;
		m_length = std::sqrt(m_length);
	}

	Real Vector::perpendicularDistance(const Vector &point) {
		Real numerator = 0, denominator = 0;
		for (size_t i = 0; i<m_data.size(); ++i){
			numerator += m_data[i] * point[i];
			denominator += m_data[i] * m_data[i];
		}
		Real k = numerator / denominator;

		Real d = 0;
		for (size_t i = 0; i<m_data.size(); i += 1)
			d += (k*m_data[i] - point[i])*(k*m_data[i] - point[i]);

		return sqrt(d);
	}

}