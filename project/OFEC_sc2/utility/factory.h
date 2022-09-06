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
* class factory is for the registration of all problems and algorithms
*
*********************************************************************************/

#ifndef OFEC_FACTORY_H
#define OFEC_FACTORY_H

#include <map>
#include <set>
#include <string>
#include <functional>
#include "myexcept.h"
#include "../core/definition.h"

namespace ofec {
	template<typename TBase>
	struct Factory {
		template<typename TDerived>
		struct register_ {
			register_(const std::string& key, std::set<ProTag>&& tag) {
				map_.emplace(
					key,
					make_pair([]() {
						return new TDerived();
						},
						std::forward<std::set<ProTag>>(tag))
				);
			}
		};

		static TBase* produce(const std::string& key) {
			auto it = map_.find(key);
			if (it == map_.end())
				throw MyExcept("the key is not exist!");
			return it->second.first();
		}

		static const std::map<std::string, std::pair<std::function<TBase*()>, std::set<ProTag>> >& get() {
			return map_;
		}

		Factory() = default;
	private:
		Factory& operator=(const Factory &) = delete;
		Factory& operator=(Factory &&) = delete;
		Factory(const Factory &) = delete;
		Factory(Factory &&) = delete;
		static std::map<std::string, std::pair<std::function<TBase* ()>, std::set<ProTag>>> map_;
	};

	template<typename TBase>
	std::map<std::string, std::pair<std::function<TBase*()>, std::set<ProTag>> > Factory<TBase>::map_;

#define REGISTER(TBase, TDerived, key, tag) Factory<TBase>::register_<TDerived> reg_##TDerived(key, tag)
}

#endif
