#include "DNSGAII_B.h"
#include "../../../../core/problem/continuous/continuous.h"
#include <algorithm>

#ifdef OFEC_DEMO
#include "../../../../../../ui/buffer/continuous/buffer_cont_MOP.h"
extern unique_ptr<Demo::scene> Demo::msp_buffer;
#endif

namespace ofec {
	DNSGAII_B_pop::DNSGAII_B_pop(size_t size_pop) : SBX_pop<>(size_pop) {}

	DNSGAII_B::DNSGAII_B(param_map & v) : algorithm(v.at("algorithm name")), m_pop(v.at("population size")) {}

	void DNSGAII_B_pop::initialize() {
		SBX_pop::initialize();
		for (auto& i : this->m_inds)	m_offspring.emplace_back(*i);
		for (auto& i : this->m_inds)	m_offspring.emplace_back(*i);
		set_detect_rate(global::ms_arg.find("detectRatio")->second);
		set_replace_rate(global::ms_arg.find("replaceRatio")->second);
		set_hypermuta_rate(global::ms_arg.find("hyperMutaRate")->second);
	}

	void DNSGAII_B::initialize() {
		m_pop.set_cr(0.9);
		m_pop.set_mr(1.0 / CONTINUOUS_CAST->variable_size());
		m_pop.set_eta(20, 20);
		m_pop.initialize();
		m_pop.evaluate();
#ifdef OFEC_DEMO
		vector<vector<Solution<>*>> pops(1);
		for (size_t i = 0; i < m_pop.size(); ++i)
			pops[0].emplace_back(&m_pop[i].solut());
		dynamic_cast<Demo::buffer_cont_MOP*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
#endif
	}

	int DNSGAII_B_pop::evolve() {
		if (this->m_inds.size() % 2 != 0)
			throw myexcept("population size should be even @NSGAII_SBXRealMu::evolve()");
		int tag = kNormalEval;
		for (size_t i = 0; i < this->m_inds.size(); i += 2) {
			std::vector<size_t> p(2);
			p[0] = tournament_selection();
			do { p[1] = tournament_selection(); } while (p[1] == p[0]);
			crossover(p[0], p[1], m_offspring[i], m_offspring[i + 1]);
			mutate(m_offspring[i]);
			mutate(m_offspring[i + 1]);
		}
		for (size_t i = 0; i < this->m_inds.size(); i++) {
			tag = m_offspring[i].evaluate();
			if (tag != kNormalEval) break;
		}

		//detect change and response it
		bool changed = problem_changed();
		if (changed)
			response_change();

		for (size_t i = 0; i < this->m_inds.size(); ++i) {
			m_offspring[i + this->m_inds.size()] = *this->m_inds[i];
		}
		survivor_selection(this->m_inds, m_offspring);
		if (changed) {
			for (size_t i = 0; i < this->m_inds.size(); i++) {
				tag = this->m_inds[i]->evaluate();      //环境变化之后重评价选择淘汰之后的IGD
				if (tag != kNormalEval) break;
			}
			//global::ms_global->m_algorithm->record();
			update_pop();//引入新个体
		}
		m_iter++;
		return tag;
	}

	void DNSGAII_B::run_() {
		while (!terminating()) {
			m_pop.evolve();
#ifdef OFEC_DEMO
			vector<vector<Solution<>*>> pops(1);
			for (size_t i = 0; i < m_pop.size(); ++i)
				pops[0].emplace_back(&m_pop[i].solut());
			dynamic_cast<Demo::buffer_cont_MOP*>(Demo::msp_buffer.get())->updateBuffer_(&pops);
#endif
		}
	}

	bool DNSGAII_B_pop::problem_changed() {
		size_t sample_num = std::floor(this->m_inds.size() * m_detect_rate);//get the number of re-evaluate individual
		//std::vector<std::unique_ptr<individual_type>> sample;//store re-evaluate individuals
		std::vector<std::vector<Real>> sample;//store re-evaluate individuals
		std::vector<size_t> index;//store the index of the re-evaluate individuals
		int tag = kNormalEval;
		for (size_t i = 0; i < sample_num;) {
			size_t ind = std::floor(this->m_inds.size() * global::ms_global->m_uniform[caller::Algorithm]->next());//采样随机个体的索引
			//保证不重复选择个体
			if (index.empty() || *(index.end() - 1) != ind) {
				index.push_back(ind);
				//sample.push_back(this->m_inds[ind]);
				sample.push_back(this->m_inds[ind]->objective());//将原有目标值保存
				tag = this->m_inds[ind]->evaluate(false, caller::Algorithm);//对选中个体进行重新评价
				for (size_t j = 0; j < global::ms_global->m_problem->objective_size(); j++) {//detect whether the objectives have been changed or not
					if (sample.back()[j] != this->m_inds[ind]->objective()[j])
						return true;
				}
				i++;
			}
		}
		return false;
	}

	void DNSGAII_B_pop::response_change() {
		int tag = kNormalEval;
		//re-evaluate the parents
		for (auto &i : this->m_inds) {
			tag = i->evaluate(false, caller::Algorithm);
			if (tag != kNormalEval) break;
		}

		//global::ms_global->m_algorithm->record();//重评价父代之后计算IGD，比较不同环境下父代分布的差异

		for (size_t i = 0; i < this->m_inds.size(); i++) {
			tag = m_offspring[i].evaluate(false, caller::Algorithm);
			if (tag != kNormalEval) break;
		}
	}

	void DNSGAII_B_pop::update_pop() {
		set_mr(0.5);
		size_t num = std::floor(this->m_inds.size() * m_replace_rate);//get the number of replace individuals
		std::vector<size_t> index;//store the index of the replace individuals
		int tag = kNormalEval;
		for (size_t i = 0; i < num;) {
			size_t ind = std::floor(this->m_inds.size() * global::ms_global->m_uniform[caller::Algorithm]->next());
			if (index.empty() || *(index.end() - 1) != ind) {
				index.push_back(ind);
				auto m_ind = *this->m_inds[ind];
				while (m_ind.same(*(this->m_inds[ind]))) {
					mutate(*(this->m_inds[ind]));
				}
				this->m_inds[ind]->evaluate(false, caller::Algorithm);
				i++;
			}
		}
		set_mr(1.0 / CONTINUOUS_CAST->variable_size());
	}

	void DNSGAII_B::record() {
		size_t evals = CONTINUOUS_CAST->evaluations();
		Real IGD = CONTINUOUS_CAST->get_optima().IGD_to_PF(m_pop);
		measure::get_measure()->record(global::ms_global.get(), evals, IGD);
	}
}