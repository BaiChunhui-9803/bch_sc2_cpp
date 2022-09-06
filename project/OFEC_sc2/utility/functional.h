
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
* some general methods used in OFEC are defined in this file, including distance
* metrics, solution domination relationship, 
*
*********************************************************************************/

#ifndef OFEC_FINCTIONAL_H
#define OFEC_FINCTIONAL_H

#include <cmath>
#include <vector>
#include "../core/definition.h"
#include <cstdio>
#include <string>
#include <list>
#include <functional>

namespace ofec {	
		/*
		distance measures between two std::vector-based points
		*/
	template<typename Iter1, typename Iter2>
	Real euclideanDistance(Iter1 first1, Iter1 last1, Iter2 first2) {
		Real dis = 0;
		while (first1 != last1) {
			dis += (*first1 - *first2)*(*first1 - *first2);
			++first1;
			++first2;
		}
		dis = sqrt(dis);
		return dis;
	}

	template<typename Iter1, typename Iter2>
	Real manhattanDistance(Iter1 first1, Iter1 last1, Iter2 first2) {
		Real dis = 0;
		while (first1 != last1) {
			dis += fabs(*first1++ - *first2++);
		}
		return dis;
	}
	


	template<typename Iter1, typename Iter2>
	int hammingDistance(Iter1 first1, Iter1 last1, Iter2 first2)  {
		int dis = 0;
		while (first1 != last1) {
			dis += *first1++ != *first2++;
		}
		return dis;
	}


	//domination relationship between two objective vectors
	template<typename T = Real >
	Dominance objectiveCompare(const std::vector<T>& a, const std::vector<T>& b, const std::vector<OptMode> &mode)  {
		if (a.size() != b.size()) 
			return Dominance::kNonComparable;

		int better = 0, worse = 0, equal = 0;
		for (decltype(a.size()) i = 0; i<a.size(); ++i) {
			if (mode[i] == OptMode::kMinimize) {
				if (a[i] < b[i]) {
					if (worse > 0)
						return Dominance::kNonDominated;
					else
						++better;
				}
				else if (a[i] > b[i]) {
					if (better > 0)
						return Dominance::kNonDominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
			else {
				if (a[i] > b[i]) {
					if (worse > 0)
						return Dominance::kNonDominated;
					else
						++better;
				}
				else if (a[i] < b[i]) {
					if (better > 0)
						return Dominance::kNonDominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
		}
		if (equal == a.size()) return Dominance::kEqual;
		else if (worse == 0) return Dominance::kDominant;
		else if (better == 0) return Dominance::kDominated;
        else return Dominance::kNonDominated;
	}

	template<typename T = Real >
	Dominance objectiveCompare(const std::vector<T> & a, const std::vector<T> & b, OptMode mode) {
		if (a.size() != b.size())
			return Dominance::kNonComparable;

		int better = 0, worse = 0, equal = 0;
		for (decltype(a.size()) i = 0; i < a.size(); ++i) {
			if (mode == OptMode::kMinimize) {
				if (a[i] < b[i]) {
					if (worse > 0)
						return Dominance::kNonDominated;
					else
						++better;
				}
				else if (a[i] > b[i]) {
					if (better > 0)
						return Dominance::kNonDominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
			else {
				if (a[i] > b[i]) {
					if (worse > 0)
						return Dominance::kNonDominated;
					else
						++better;
				}
				else if (a[i] < b[i]) {
					if (better > 0)
						return Dominance::kNonDominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
		}
		if (equal == a.size()) return Dominance::kEqual;
		else if (worse == 0) return Dominance::kDominant;
		else if (better == 0) return Dominance::kDominated;
        else return Dominance::kNonDominated;
	}

	//add by tanqingshan. return the first rank points.
	template <typename T=Real>
	std::vector<std::vector<T>*> & get_nondominated_set(std::vector<std::vector<T>*> &data, OptMode opt_mode) {
		size_t n = data.size();
		std::vector<std::vector<T>*> obj = data;
		data.clear();
		data.push_back(obj[0]);
		for (size_t i = 1; i < n; i++) {
			size_t m = data.size();
			for (size_t j = 0; j < m; j++) {
				Dominance temp_ = objectiveCompare(*obj[i], *data[j], opt_mode);
				if (temp_ == Dominance::kEqual) {
					data.push_back(obj[i]);
				}
				else if (temp_ == Dominance::kDominant) {
					data[j] = obj[i];
					break;
				}
				else if (temp_ == Dominance::kNonDominated) {
					if (j == m - 1)
						data.push_back(obj[i]);
					else
						continue;
				}
				else {
					continue;
				}
			}
		}
		return data;
	}

	template <typename T>
	int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}

	template<typename T>
	bool less(const T &d1, const T &d2, bool ascending) {
		if (ascending) {
			if (d1 < d2) return true;
			else return false;
		}
		else {
			if (d1 < d2) return false;
			else return true;
		}
	}

	template<typename T>
	bool less(T *d1, T *d2, bool ascending) {
		if (ascending) {
			if (*d1 < *d2) return true;
			else return false;
		}
		else {
			if (*d1 < *d2) return false;
			else return true;
		}
	}

	namespace merge_sort_func {
		template<typename T>
		void merge(const T& data, std::vector<int>& A, std::vector<int>& B, int l, int m, int r, bool ascending) {
			int i = l, j = m;
			for (int k = l; k < r; ++k) {
				if (i < m && (j >= r || less(data[A[i]], data[A[j]], ascending))) {
					B[k] = A[i];
					i++;
				}
				else {
					B[k] = A[j];
					j++;
				}
			}
		}

		template<typename T>
		void sort(const T& data, std::vector<int>& B, std::vector<int>& A, int l, int r, bool ascending){
			if (r - l < 2)
				return;
			int m = (r + l) / 2;
			sort(data, A, B, l, m, ascending);
			sort(data, A, B, m, r, ascending);
			merge(data, B, A, l, m, r, ascending);
		}
	}

	/* index[i] denotes the index of the i-th minimum (in ascending order) element */
	template<typename T>
	void mergeSort(const T &data, int size, std::vector<int>& index, bool ascending = true, int low = 0, int up = -1, int num = -1, bool start = true) {
		if (start) {
			if (up == -1) up = size - 1;
			if (num == -1) num = size;
			if (index.size() == 0 || index.size() != size)		index.resize(size);
			for (auto i = index.begin(); i != index.end(); ++i) *i = i - index.begin();
		}
		if (low >= up) return;

		std::vector<int> B(index);
		merge_sort_func::sort(data, B, index, low, up + 1, ascending);
	}

	template <class T>
	std::vector<int> amend_order(const T &data, const std::vector<int> &index) {
		/*amend index in the case of the same item values in data. Note: data must be
		sorted with results stored in index, e.g.,
		data[]=[5,2,2,1]; index[]=[3,2,1,0]; //ascending order
		after amendation  index[]=[2,1,1,0];
		*/
		std::vector<int> result(index.size());
		for (int r = 1, idx = 1; r <= index.size(); r++, idx++) {
			int temp = r, count = 1;
			while (temp<index.size() &&data[index[temp - 1]] == data[index[temp]]) {
				count++; 
				temp++; 
			}
			for (int k = 0; k<count; k++) result[index[r + k - 1]] = idx;
			r += count - 1;
		}
		return result;
	}
	template<typename T>
	inline T chaotic_value(T x, T min, T max, T rChaoticConstant = 3.54) {
		// return a value calculated by logistic map
		if (min>max) return -1;
		T val;
		val = (x - min) / (max - min);
		val = rChaoticConstant*val*(1 - val);
		return min + val*(max - min);
	}

	inline bool exists(const std::string& name) {
		if (FILE *file = fopen(name.c_str(), "r")) {
			fclose(file);
			return true;
		}
		else {
			return false;
		}
	}
	template<typename T>
	inline T mapReal(T value, T input_min, T input_max, T output_min, T output_max) {
		return ((value - input_min) / (input_max - input_min) * (output_max - output_min) + output_min);
	}
	template<typename T,typename K>
	inline void calMeanAndStd(const std::vector<T> &data, K&mean, K&dev) {
		mean = 0;
		for (T val : data)
			mean += val;
		mean /= static_cast<K>(data.size());
		dev = 0;
		for (T val : data)
			dev += (val - mean) * (val - mean);
		dev = sqrt(dev / static_cast<K>(data.size()));
	}

	template<typename T>
	inline int calIdx(const std::vector<T>& inds, const std::function<bool(const T&, const T&)>& comp) {

		if (inds.empty()) return -1;
		int bestIdx(0);
		for (int curIdx(0); curIdx < inds.size(); ++curIdx) {
			if (comp(inds[curIdx], inds[bestIdx])) {
				bestIdx = curIdx;
			}
		}
		return bestIdx;
	}
	

	template<typename T>
	inline std::list<int> calBestIdx(const std::vector<T>& inds, const std::function<Dominance(const T&, const T&)>& comp) {
		
		if (inds.empty()) return std::list<int>();
		std::list<int> bestIdx;
		
		for (int idx(0); idx < inds.size(); ++idx) {
			bool flag = true;
			for (auto bestIter = bestIdx.begin(); bestIter != bestIdx.end(); ) {
				auto com_flag = comp(inds[idx], inds[*bestIter]);
				if (com_flag == Dominance::Dominated) {
					flag = false;
					break;
				}
				else if(com_flag==Dominance::Dominant) {
					bestIter=bestIdx.erase(bestIter);
				}
				else {
					bestIter++;
				}
			}
			if (flag) {
				bestIdx.push_back(idx);
			}
		}

		return bestIdx;
		
	}

	template<typename T,typename K>
	inline bool updateBestSol( std::vector<T>& inds, const K & newly_one ,std::function<Dominance(const K&, const T&)>& comp) {
		std::vector<bool> flag(inds.size(), true);
		bool newly_one_flag = true;
		bool erase_flag = false;
		for (int idx(0); idx < inds.size(); ++idx) {
			auto com_flag = comp(newly_one,inds[idx]);
			if (com_flag == Dominance::Dominated) {
				newly_one_flag = false;
				break;
			}
			else if (com_flag == Dominance::Dominant) {
				flag[idx] = false;
				erase_flag = true;
			}
		}
		if (newly_one_flag) {
			if (erase_flag) {
				int cur_idx(0);
				for (int idx(0); idx < inds.size(); ++idx) {
					if (flag[idx]) {
						if (cur_idx != idx) {
							std::swap(inds[idx], inds[cur_idx]);
						}
						++cur_idx;
					}
				}
				inds.resize(cur_idx);
				return true;
			}
			else {
				return true;
			}
		}
		return false;
	}
}
#endif // !OFEC_FINCTIONAL_H

