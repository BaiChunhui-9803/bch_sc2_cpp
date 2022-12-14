#ifndef SOLUTION_H
#define SOLUTION_H

#define CommandSize 10				//一个解的命令长度

#include <sc2api/sc2_api.h>
#include <vector>
#include <functional>
#include <cassert>
#include <sc2api/sc2_common.h> //! I want to use the random functions. They are easy to use and I love them!
#include <numeric>
#include "Command.h"

namespace sc2 {
	// many commands make up a sulution
	struct Solution {
		// 目标值
		//std::vector<float> s_objectives;
		float s_objectives;
		// 变量
		Command s_commands;
		int s_rank = 0;

		bool operator==(const Solution& rhs) const {
			return s_commands == rhs.s_commands;
		}
		bool operator!=(const Solution& rhs) const {
			return !(*this == rhs);
		}

		Solution() {
			s_commands.c_actions.resize(2 * CommandSize);
			s_objectives = 0;
		}
		~Solution() = default;
		Solution(int command_size) {
			s_commands.c_actions.resize(command_size);
			s_objectives = 0;
		}

		//! two compares for the use of sort(), descending order
		// return true means the orders of the two items keep unchanged
		//static bool multi_greater(const Solution& a, const Solution& b) {
		//	bool equal = true;
		//	// If any of the b's dimensions is bigger than a, return false
		//	for (size_t i = 0; i < a.s_objectives.size(); i++) {
		//		if (a.s_objectives[i] < b.s_objectives[i]) {
		//			return false;
		//		}
		//		else if (equal && fabsf(a.s_objectives[i] - b.s_objectives[i]) > 0.000001) {
		//			equal = false;
		//		}
		//	}
		//	// If all the demensions are equal, return false
		//	if (equal) {
		//		return false;
		//	}
		//	// else return true
		//	return true;
		//}
		// Simply add up all the objectives without considering weights
		//static bool simple_sum_greater(const Solution& a, const Solution& b) {
		//	return std::accumulate(a.s_objectives.begin(), a.s_objectives.end(), 0.f) > std::accumulate(b.s_objectives.begin(), b.s_objectives.end(), 0.f);
		//}
	};
}

#endif