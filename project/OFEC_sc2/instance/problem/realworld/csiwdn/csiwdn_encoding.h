#ifndef OFEC_CSIWDN_ENCODING_H
#define OFEC_CSIWDN_ENCODING_H

#include <vector>
#include <functional>

#include "../../../../core/problem/encoding.h"

namespace ofec {
	class VarCSIWDN : public VarBase{
	protected:
		struct epa_type {
			int Interval = 0;
			bool isLoc = true;
			/*bool isDetected = false;*/
			long first_detected_time;
			char Loc[32];
			int Index;
			float Source = 1.0;
			long startTime;
			long Duration;
			std::vector<Real> Multiplier;
		};
		std::vector<epa_type> m_x;

	public:
		VarCSIWDN() {};
		VarCSIWDN(size_t size) :m_x(size) {}
		VarCSIWDN(const VarCSIWDN& rhs) : m_x(rhs.m_x) {}
		VarCSIWDN(VarCSIWDN&& rhs) :m_x(std::move(rhs.m_x)) {}

		VarCSIWDN& operator=(const VarCSIWDN& rhs) {
			if (this == &rhs) return *this;
			m_x = rhs.m_x;
			return *this;
		}

		VarCSIWDN& operator=(VarCSIWDN&& rhs) {
			m_x = std::move(rhs.m_x);
			return *this;
		}

		size_t numberSource() const {
			return m_x.size();
		}

		void append(const epa_type x) {
			m_x.push_back(x);
		}

		void resizeEpaSet(size_t n) {
			m_x.resize(n);
		}

		epa_type& getEpa(size_t i) {
			return m_x[i];
		}

		char* location(size_t i) {
			return m_x[i].Loc;
		}

		long& duration(size_t i) {
			return m_x[i].Duration;
		}

		long duration(size_t i) const {
			return m_x[i].Duration;
		}

		float& source(size_t i) {
			return m_x[i].Source;
		}

		long& startTime(size_t i) {
			return m_x[i].startTime;
		}

		long startTime(size_t i) const {
			return m_x[i].startTime;
		}

		std::vector<Real>& multiplier(size_t i) {
			return m_x[i].Multiplier;
		}

		const std::vector<Real>& multiplier(size_t i) const {
			return m_x[i].Multiplier;
		}

		bool& flagLocation(size_t i) {
			return m_x[i].isLoc;
		}

		int& index(size_t i) {
			return m_x[i].Index;
		}

		int index(size_t i) const {
			return m_x[i].Index;
		}

		/*bool& isDetected(size_t i) {
			return m_x[i].isDetected;
		}*/
		
		int& interval(size_t i) {
			return m_x[i].Interval;
		}
		
		long& firstDetectedTime(size_t i) {
			return m_x[i].first_detected_time;
		}
		
		size_t size() const noexcept {
			return 1;
		}
		
		void resize(size_t n) {
			m_x.resize(n);
		}
	};
}

#endif //!OFEC_CSIWDN_ENCODING_H
