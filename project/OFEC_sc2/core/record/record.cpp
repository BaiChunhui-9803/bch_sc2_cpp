#include <fstream>
#include <algorithm>
#include <cmath>
#include "record.h"
#include "../../core/global.h"

namespace ofec {
	Record::Record(const ParamMap &param) : m_params(param) {
		setFileName();
	}

	void Record::setFileName() {
		m_filename.str("");
		m_filename << g_working_dir << "/result/";
		std::vector<std::string> abbrv_args;
		for (auto &i : m_params) {
			if (g_param_omit.count(i.first) == 0) {
				for (auto &j : g_param_abbr) {
					if (i.first == j.second) {
						abbrv_args.push_back(j.first);
						break;
					}
				}
			}
		}
		std::sort(abbrv_args.begin(), abbrv_args.end());
		for (const auto &arg : abbrv_args) {
			for (auto &i : m_params) {
				if (i.first == g_param_abbr.at(arg)) {
					m_filename << arg << "(" << i.second << ")_";
					break;
				}
			}
		}
	}
}