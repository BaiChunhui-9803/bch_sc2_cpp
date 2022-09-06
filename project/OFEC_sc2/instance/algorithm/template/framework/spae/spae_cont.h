#ifndef OFEC_SPAE_CONT_H
#define OFEC_SPAE_CONT_H

#include "hlc.h"
#include "../../../../../core/algorithm/multi_population.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../record/rcr_vec_real.h"
#include "../../../../problem/continuous/global/CEC2005/F1_shifted_sphere.h"
#include "../../../../../core/instance_manager.h"
#include "../../../../../core/global.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
// include buffer
#endif

namespace ofec {
	class BaseContSPAE : public Algorithm {
	public:
		enum class Stage { explore, cluster, exploit };

	protected:
		std::unique_ptr<HLC> m_hlc;
		size_t m_subpop_size;
		int m_init_num_ssp;
		std::vector<std::pair<Real, Real>> m_base_cov_rate;
		std::vector<std::pair<Real, Real>> m_base_cml_cov_rate;
		std::pair<Real, Real> m_base_log_base;
		int m_cur_bsn;
		Stage m_cur_stage;
		std::vector<std::pair<Real, Real>> m_fitness_distance;

		void initialize_() override {
			Algorithm::initialize_();
			const auto &v = GET_PARAM(m_id_param);
			m_subpop_size = std::get<int>(v.at("subpopulation size"));
			m_init_num_ssp = v.count("initial number of subspaces") > 0 ? std::get<int>(v.at("initial number of subspaces")) : 1;

			m_hlc.reset(new HLC(GET_CONOP(m_id_pro).numVariables(), m_subpop_size));
			m_hlc->initialize(GET_CONOP(m_id_pro).boundary(), GET_PRO(m_id_pro).optMode(0), m_init_num_ssp);
		}

#ifdef OFEC_DEMO
	public:
		const HLC& getHLC() const { return *m_hlc; }
		int curBsn() const { return m_cur_bsn; }
		Stage curStage() const { return m_cur_stage; }
		const std::vector<std::pair<Real, Real>>& baseCovRate() const { return m_base_cov_rate; }
		const std::vector<std::pair<Real, Real>>& baseCmlCovRate() const { return m_base_cml_cov_rate; }
		const std::pair<Real, Real>& baseLogBase() const { return m_base_log_base; }
		const std::vector<std::pair<Real, Real>> fitnessDistance() const { return m_fitness_distance; }
#endif	
	};

	template<typename PopType>
	class ContSPAE : public BaseContSPAE {
	protected:
		std::string m_pop_type;
		MultiPopulation<PopType> m_multi_pop;
		Real m_rstrct_fctr;
		bool m_init_clstr;
		Real m_var_niche_radius;

		using IndType = typename PopType::IndType;
		std::vector<size_t> m_affil_bsn;
		std::vector<Real> m_niche_radius;
		std::list<std::unique_ptr<Solution<>>> m_converged;

	public:
		ContSPAE() = default;
		void record() override;

	protected:
		void initialize_() override;
		void run_() override;
#ifdef OFEC_DEMO
		void updateBuffer();
#endif	

		virtual void setPopType() = 0;
		virtual void initMultiPop();
		virtual bool isPopConverged(PopType &pop);
		virtual bool isPopConvergeOver(PopType &pop);
		virtual void initSubPop(PopType &pop, size_t id_bsn_atr, HLC &hlc, int id_pro, int id_alg, int id_rnd);
		virtual void evolveSubPop(PopType &pop);
		void calCoverage(PopType &pop, size_t id_bsn_atr, Real niche_radius, HLC &hlc, int id_pro);

		virtual void restrictPop(PopType &pop, size_t id_bsn_atr);
		virtual void initInd(HLC& hlc, int id_pro, int id_alg, size_t id_bsn_atr, size_t id_div, IndType &ind);
		Real calNicheRadius(const PopType &pop, int id_pro) const;

	private:
		void loadBaseCovLn();
		void calBaseCovLn();
	};

	template<typename PopType>
	void ContSPAE<PopType>::loadBaseCovLn() {
		std::string file_name = g_working_dir + "/instance/algorithm/template/framework/spae/data/"
			+ m_pop_type + "(SpS=" + std::to_string(m_subpop_size) + ").hlc";
		std::ifstream infile(file_name);
		if (!infile)
			calBaseCovLn();
		else {
			for (std::string line; std::getline(infile, line);) {
				if (line.find(":") != std::string::npos) {
					auto name = line.substr(0, line.find(':'));
					auto value = line.substr(line.find(':') + 2, line.size() - 1);
					if (name == "Mean of log base")
						m_base_log_base.first = std::stod(value);
					else if (name == "Std of log base")
						m_base_log_base.second = std::stod(value);
					else if (name == "Mean and Std of coverage rates") {
						for (std::stringstream line_stream(value); line_stream; ) {
							m_base_cov_rate.resize(m_base_cov_rate.size() + 1);
							line_stream >> m_base_cov_rate.back().first >> m_base_cov_rate.back().second;
						}
						m_base_cov_rate.pop_back();
					}
					else if (name == "Mean and Std of cumulative coverage rates") {
						for (std::stringstream line_stream(value); line_stream; ) {
							m_base_cml_cov_rate.resize(m_base_cml_cov_rate.size() + 1);
							line_stream >> m_base_cml_cov_rate.back().first >> m_base_cml_cov_rate.back().second;
						}
						m_base_cml_cov_rate.pop_back();
					}
				}
			}
		}
	}

	template<typename PopType>
	void ContSPAE<PopType>::calBaseCovLn() {
		std::unique_ptr<Problem> pro(new CEC2005_GOP_F01);
		ParamMap v;
		v["problem name"] = std::string("GOP_CEC2005_F01");
		v["number of variables"] = (int)GET_CONOP(m_id_pro).numVariables();
		size_t num_runs = 50;
		int id_param(ADD_PARAM(v));
		int id_pro = ADD_PRO_EXST(id_param, pro, 0.5);
		GET_CONOP(id_pro).initialize();

		std::vector<std::vector<Real>> data_cov_rate;
		std::vector<std::vector<Real>> data_cml_cov_rate;
		std::vector<Real> data_log_param;
		size_t max_len = 0;
		for (size_t id_run = 0; id_run < num_runs; id_run++) {
			HLC h(GET_CONOP(id_pro).numVariables(), m_subpop_size);
			h.initialize(GET_CONOP(id_pro).boundary(), GET_CONOP(id_pro).optMode(0), 1);
			PopType p(m_subpop_size, id_pro);
			initSubPop(p, 0, h, id_pro, -1, m_id_rnd);
			auto nr = calNicheRadius(p, id_pro);
			calCoverage(p, 0, nr, h, id_pro);
			Real cov_before = -1;
			size_t num_cov_remain = 0;
			while (!isPopConverged(p)) {
				p.evolve(id_pro, -1, m_id_rnd);
				calCoverage(p, 0, nr, h, id_pro);
				if (h.infoBsnAtrct(0).coverages.back() == cov_before)
					num_cov_remain++;
				else {
					cov_before = h.infoBsnAtrct(0).coverages.back();
					num_cov_remain = 0;
				}
				if (num_cov_remain > 5)
					break;
			}
			h.regressCovLines();
			data_cov_rate.push_back(h.infoBsnAtrct(0).coverages);
			data_cml_cov_rate.push_back(h.infoBsnAtrct(0).cuml_covs);
			data_log_param.push_back(h.infoBsnAtrct(0).log_base);
			if (max_len < h.infoBsnAtrct(0).coverages.size())
				max_len = h.infoBsnAtrct(0).coverages.size();
		}
		m_base_cov_rate.resize(max_len);
		for (auto &row : data_cov_rate) {
			while (row.size() < max_len)
				row.push_back(row.back());
		}
		for (size_t j = 0; j < max_len; j++) {
			std::vector<Real> col;
			for (auto &row : data_cov_rate)
				col.push_back(row[j]);
			calMeanAndStd(col, m_base_cov_rate[j].first, m_base_cov_rate[j].second);
		}
		m_base_cml_cov_rate.resize(max_len);
		for (auto &row : data_cml_cov_rate) {
			while (row.size() < max_len)
				row.push_back(row.back());
		}
		for (size_t j = 0; j < max_len; j++) {
			std::vector<Real> col;
			for (auto &row : data_cml_cov_rate)
				col.push_back(row[j]);
			calMeanAndStd(col, m_base_cml_cov_rate[j].first, m_base_cml_cov_rate[j].second);
		}
		calMeanAndStd(data_log_param, m_base_log_base.first, m_base_log_base.second);

		std::stringstream out_stream;
		out_stream << "Mean of log base: " << m_base_log_base.first;
		out_stream << "\nStd of log base: " << m_base_log_base.second;
		out_stream << "\nMean and Std of coverage rates:";
		for (auto &c : m_base_cov_rate)
			out_stream << " " << c.first << " " << c.second;
		out_stream << "\nMean and Std of cumulative coverage rates:";
		for (auto &c : m_base_cml_cov_rate)
			out_stream << " " << c.first << " " << c.second;

		std::string file_name = g_working_dir + "/instance/algorithm/template/framework/spae/data/"
			+ m_pop_type + "(SpS=" + std::to_string(m_subpop_size) + ").hlc";
		std::ofstream out_file(file_name);
		out_file << out_stream.str();
		out_file.close();

		DEL_PRO(id_pro);
		DEL_PARAM(id_param);
	}

	template<typename PopType>
	void ContSPAE<PopType>::initMultiPop() {
		if (m_init_clstr) {
			size_t num_each_ssp = 5;
			size_t num_objs = GET_PRO(m_id_pro).numObjectives();
			size_t num_cons = GET_PRO(m_id_pro).numConstraints();
			size_t num_vars = GET_CONOP(m_id_pro).numVariables();
			Solution<> temp_sol(num_objs, num_cons, num_vars);
			for (size_t i = 0; i < m_init_num_ssp; i++) {
				for (size_t k = 0; k < num_each_ssp; k++) {
					m_hlc->randomSmplInSSP(i, m_id_rnd, temp_sol.variable().vect());
					temp_sol.evaluate(m_id_pro, m_id_alg);
					m_hlc->inputSample(temp_sol);
				}
			}
			m_hlc->clustering();
		}
#ifdef OFEC_DEMO
		updateBuffer();
#endif
		for (size_t id_bsn = 0; id_bsn < m_hlc->numBsnsAtrct(); id_bsn++) {
			size_t id_pop = m_multi_pop.size();
			auto pop = std::make_unique<PopType>(m_subpop_size, m_id_pro);
			m_multi_pop.append(pop);
			initSubPop(m_multi_pop[id_pop], id_bsn, *m_hlc, m_id_pro, m_id_alg, m_id_rnd);
			m_niche_radius.push_back(calNicheRadius(m_multi_pop[id_pop], m_id_pro));
			calCoverage(m_multi_pop[id_pop], id_bsn, m_niche_radius[id_pop], *m_hlc, m_id_pro);
			m_multi_pop[id_pop].setActive(true);
			m_affil_bsn.push_back(id_bsn);
		}

#ifdef OFEC_DEMO
		updateBuffer();
#endif
	}

	template<typename PopType>
	Real ContSPAE<PopType>::calNicheRadius(const PopType &pop, int id_pro) const {
		Real dis, min_dis = pop[0].variableDistance(pop[1], id_pro);
		for (size_t i = 0; i < pop.size(); i++)	{
			for (size_t j = i+1; j < pop.size(); j++) {
				dis = pop[i].variableDistance(pop[j], id_pro);
				if (min_dis > dis)
					min_dis = dis;
			}
		}
		return min_dis;
	}

	template<typename PopType>
	void ContSPAE<PopType>::initialize_() {
		BaseContSPAE::initialize_();
		const auto &v = GET_PARAM(m_id_param);
		m_rstrct_fctr = v.count("restriction factor") > 0 ? std::get<Real>(v.at("restriction factor")) : 1;
		m_init_clstr = v.count("initial clustering") > 0 ? std::get<bool>(v.at("initial clustering")) : false;
		m_var_niche_radius = 0;
		for (size_t j = 0; j < GET_CONOP(m_id_pro).numVariables(); j++)
			m_var_niche_radius += GET_CONOP(m_id_pro).range(j).second - GET_CONOP(m_id_pro).range(j).first;
		m_var_niche_radius *= 1e-3;
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	template<typename PopType>
	void ContSPAE<PopType>::record() {
		std::vector<Real> entry;
		entry.push_back(m_effective_eval);
		if (GET_PRO(m_id_pro).hasTag(ProTag::kMMOP)) {
			size_t num_optima_found = GET_PRO(m_id_pro).numOptimaFound(m_candidates);
			size_t num_optima = GET_CONOP(m_id_pro).getOptima().numberObjectives();
			entry.push_back(num_optima_found);
			entry.push_back(num_optima_found == num_optima ? 1 : 0);
		}
		else
			entry.push_back(m_candidates.front()->objectiveDistance(GET_CONOP(m_id_pro).getOptima().objective(0)));
		dynamic_cast<RecordVecReal &>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

#ifdef OFEC_DEMO
	template<typename PopType>
	void ContSPAE<PopType>::updateBuffer() {
		if (Demo::g_buffer->idAlg() == m_id_alg) {
			m_solution.clear();
			m_solution.resize(m_multi_pop.size());
			for (size_t k = 0; k < m_multi_pop.size(); ++k) {
				for (size_t i = 0; i < m_multi_pop[k].size(); ++i) {
					m_solution[k].push_back(&m_multi_pop[k][i].phenotype());
				}
			}			
			
			Demo::g_buffer->appendAlgBuffer(m_id_alg);

		}
	}
#endif

	template<typename PopType>
	bool ContSPAE<PopType>::isPopConverged(PopType &pop) {
		bool flag = false;
		
		Real min_obj, max_obj;
		min_obj = max_obj = pop[0].phenotype().objective(0);
		for (size_t i = 1; i < pop.size(); i++)	{
			if (pop[i].phenotype().objective(0) < min_obj)
				min_obj = pop[i].phenotype().objective(0);
			if (pop[i].phenotype().objective(0) > max_obj)
				max_obj = pop[i].phenotype().objective(0);
		}
		if ((max_obj - min_obj) < 0.1 * GET_PRO(m_id_pro).objectiveAccuracy())
			flag = true;

		if (!flag) {
			if (pop.iteration() - pop.timeBestUpdated() > 100)
				flag = true;
		}

		return flag;
	}

	template<typename PopType>
	bool ContSPAE<PopType>::isPopConvergeOver(PopType &pop) {
		for (size_t i = 0; i < pop.size(); i++)	{
			if (pop[i].type() == 2)
				continue;
			for (auto& co : m_converged) {
				if (pop[i].phenotype().variableDistance(*co, m_id_pro) < m_var_niche_radius) {
					pop[i].setType(2);
					break;
				}
			}
		}
		for (size_t i = 0; i < pop.size(); i++)
			if (pop[i].type() != 2)
				return false;
		return true;
	}

	template<typename PopType>
	void ContSPAE<PopType>::initSubPop(PopType &pop, size_t id_bsn_atr, HLC &hlc, int id_pro, int id_alg, int id_rnd) {
		std::vector<bool> div_converged(pop.size(), false);
		for (auto &s : m_converged)	{
			auto aff_div = m_hlc->getIdxDiv(id_bsn_atr, *s);
			if (aff_div > -1)
				div_converged[aff_div] = true;
		}
		for (size_t i = 0; i < pop.size(); i++) {
			size_t id_div = i;
			while (div_converged[id_div])
				id_div = GET_RND(m_id_rnd).uniform.nextNonStd(0, pop.size());
			initInd(hlc, id_pro, id_alg, id_bsn_atr, id_div, pop[i]);
			pop[i].setType(0);
		}
	}

	template<typename PopType>
	void ContSPAE<PopType>::evolveSubPop(PopType &pop) {
		pop.evolve(m_id_pro, m_id_alg, m_id_rnd);
		for (size_t i = 0; i < pop.size(); ++i)
			m_hlc->inputSample(pop[i].solut());
	}

	template<typename PopType>
	void ContSPAE<PopType>::run_() {
		setPopType();
		//loadBaseCovLn();
		initMultiPop();
		int iter = 0;
		while (!terminating()) {
			while (!terminating() && m_multi_pop.isActive()) {
				for (size_t k = 0; k < m_multi_pop.size(); k++) {
					if (m_multi_pop[k].isActive()) {
						evolveSubPop(m_multi_pop[k]);
						restrictPop(m_multi_pop[k], m_affil_bsn[k]);
						if (isPopConvergeOver(m_multi_pop[k]))
							m_multi_pop[k].setActive(false);
						if (isPopConverged(m_multi_pop[k])) {
							m_multi_pop[k].setActive(false);
							int best = 0;
							for (size_t i = 1; i < m_multi_pop[k].size(); i++) {
								if (m_multi_pop[k][i].phenotype().dominate(m_multi_pop[k][best].phenotype(), m_id_pro))
									best = i;
							}
							m_converged.emplace_back(new Solution<>(m_multi_pop[k][best].phenotype()));
						}
						if (m_multi_pop[k].iteration() < m_base_cov_rate.size())
							calCoverage(m_multi_pop[k], m_affil_bsn[k], m_niche_radius[k], *m_hlc, m_id_pro);
					}
				}
#ifdef OFEC_DEMO   
				updateBuffer();
#endif
				iter++;
			}
#ifdef OFEC_DEMO   
			updateBuffer();
#endif
			//m_hlc->regressCovLines();
#ifdef OFEC_DEMO  
			m_hlc->setLogParamReady(true);
			updateBuffer();
			m_hlc->setLogParamReady(false);
#endif
//			for (size_t k = 0; k < m_multi_pop.size(); ++k) {
//				m_hlc->calRuggedness(m_affil_bsn[k], m_id_rnd, m_id_pro, m_id_alg);
//				//if (m_hlc->infoBsnAtrct(m_affil_bsn[k]).ruggedness > 0.5)
//				//	continue;
//				size_t num_pre = m_hlc->infoBsnAtrct(m_affil_bsn[k]).subspaces.size();
//				auto times = pow(log(m_base_log_base.first) / log(m_hlc->infoBsnAtrct(m_affil_bsn[k]).log_base), 4);
//				size_t num_new = ceil(num_pre * times);
//				m_hlc->splitBasin(m_affil_bsn[k], num_new);
//#ifdef OFEC_DEMO
//				if (Demo::g_term_alg)
//					return;
//#endif
//			}
#ifdef OFEC_DEMO   
			updateBuffer();
#endif
			m_hlc->interpolate();
#ifdef OFEC_DEMO   
			updateBuffer();
#endif
			m_hlc->clustering();
#ifdef OFEC_DEMO
			if (Demo::g_term_alg) return;
			updateBuffer();
#endif
			m_hlc->updatePotential(m_id_pro, m_id_rnd);
			m_multi_pop.clear();
			m_affil_bsn.clear();
			m_niche_radius.clear();
			for (size_t id_bsn = 0; id_bsn < m_hlc->numBsnsAtrct(); id_bsn++) {
				if (GET_RND(m_id_rnd).uniform.next() < m_hlc->infoBsnAtrct(id_bsn).potential) {
					size_t id_pop = m_multi_pop.size();
					auto pop = std::make_unique<PopType>(m_subpop_size, m_id_pro);
					m_multi_pop.append(pop);
					initSubPop(m_multi_pop[id_pop], id_bsn, *m_hlc, m_id_pro, m_id_alg, m_id_rnd);
					m_niche_radius.push_back(calNicheRadius(m_multi_pop[id_pop], m_id_pro));
					calCoverage(m_multi_pop[id_pop], id_bsn, m_niche_radius[id_pop], *m_hlc, m_id_pro);
					m_multi_pop[id_pop].setActive(true);
					m_affil_bsn.push_back(id_bsn);	
				}
			}

			iter++;
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		}
	}

	template<typename PopType>
	void ContSPAE<PopType>::calCoverage(PopType &pop, size_t id_bsn_atr, Real niche_radius, HLC &hlc, int id_pro) {
		std::vector<const Solution<> *> sub_pop;
		for (size_t i = 0; i < pop.size(); i++) {
			if (pop[i].type() != 1) // 0: normal, 1: reinitialized, 2: converge over
				sub_pop.push_back(&pop[i].phenotype());
		}
		hlc.calCoverage(id_bsn_atr, sub_pop, id_pro, niche_radius);
	}

	template<typename PopType>
	void ContSPAE<PopType>::restrictPop(PopType &pop, size_t id_bsn_atr) {
		size_t affiliatedSubspace, id_div;
		size_t id_ssp, idasp;
		for (size_t i = 0; i < pop.size(); i++) {
			affiliatedSubspace = m_hlc->subspaceTree().getRegionIdx(pop[i].phenotype().variable().vect());
			auto best = m_hlc->infoBsnAtrct(id_bsn_atr).best_sol;
			if (m_hlc->infoSubspace(affiliatedSubspace).aff_bsn_atr != id_bsn_atr) {
				if (pop.iteration() < 50 || GET_RND(m_id_rnd).uniform.next() < pow(1. / (pop.iteration()-50), m_rstrct_fctr)) {
					for (size_t j = 0; j < GET_CONOP(m_id_pro).numVariables(); j++)	{
						pop[i].variable()[j] = GET_RND(m_id_rnd).normal.nextNonStd(best->variable()[j], m_var_niche_radius);
					}
					pop[i].evaluate(m_id_pro, m_id_alg);
					m_hlc->inputSample(pop[i].solut());
					//id_div = m_hlc->getIdxDiv(id_bsn_atr, *m_hlc->infoBsnAtrct(id_bsn_atr).best_sol);
					//id_div = GET_RND(m_id_rnd).uniform.nextNonStd<size_t>(0, m_subpop_size);
					//initInd(*m_hlc, m_id_pro, m_id_alg, id_bsn_atr, id_div, pop[i]);
					//pop[i].setType(1);
				}
			}
		}
	}

	template<typename PopType>
	void ContSPAE<PopType>::initInd(HLC &hlc, int id_pro, int id_alg, size_t id_bsn_atr, size_t id_div, IndType &ind) {
		hlc.randomSmpl(id_bsn_atr, id_div, m_id_rnd, ind.variable().vect());
		ind.evaluate(id_pro, id_alg);
		hlc.inputSample(ind.solut());
	}
}

#endif // !OFEC_SPAE_CONT_H
