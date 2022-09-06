
#include "CDG_MOEA.h"
#include "../../../../utility/nondominated_sorting/fast_sort.h"
#include "../../../record/rcr_vec_real.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace ofec {
	/******************************CDGMOEA***********************************/
	void CDGMOEA::initialize_() {
		Algorithm::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_pop_size = std::get<int>(v.at("population size"));
		m_pop.reset();
	}

	void CDGMOEA::record() {
		std::vector<Real> entry;
		entry.push_back(m_effective_eval);
		Real IGD = GET_CONOP(m_id_pro).getOptima().invertGenDist(*m_pop);
		entry.push_back(IGD);
		dynamic_cast<RecordVecReal &>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

#ifdef OFEC_DEMO
	void CDGMOEA::updateBuffer() {
		m_solution.clear();
		m_solution.resize(1);
		for (size_t i = 0; i < m_pop->size(); ++i)
			m_solution[0].push_back(&m_pop->at(i).phenotype());
		Demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif

	void CDGMOEA::initPop() {
		m_pop.reset(new PopCDGMOEA(m_pop_size, m_id_pro));
		m_pop->initialize(m_id_pro, m_id_rnd);
		m_pop->evaluate(m_id_pro, m_id_alg);
	}

	void CDGMOEA::run_() {
		initPop();
#ifdef OFEC_DEMO
		updateBuffer();
#endif
		while (!terminating()) {
			m_pop->evolve(m_id_pro, m_id_alg, m_id_rnd);
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		}
	}

	/****************************** population ***********************************/
	PopCDGMOEA::PopCDGMOEA(size_t size_pop, int id_pro) : PopMODE(size_pop, id_pro) {
		//m_offspring.resize(2 * size_pop);
		int numobj = GET_PRO(id_pro).numObjectives();
		if (numobj == 2) {
			m_grid_div = 180;
			m_T = 5;
		}
		else if (numobj == 3) {
			m_grid_div = 30;
			m_T = 1;
		}
		m_ideal.resize(numobj), m_nadir.resize(numobj), m_grid_distance.resize(numobj), m_grid_min_max.resize(numobj),
			m_S.resize(numobj, std::vector<std::vector<int>>(pow(m_grid_div, numobj - 1)));// m_R.resize(2 * size_pop, std::vector<int>(M));
		for (int i = 0; i < numobj; ++i) {
			m_ideal[i] = m_nadir[i] = m_inds[0]->objective()[i];
		}
	}

	void PopCDGMOEA::initialize(int id_pro, int id_rnd) {
		Population<IndCDGMOEA>::initialize(id_pro, id_rnd);
		setParameter(1.0, 0.5); //set parmeter: CR=1.0, F=0.5
		updateIdealPoint(id_pro);
		updateNadirPoint(id_pro);
		gridConstruct(id_pro);
		assign_GN(id_pro);
		for (auto &i : m_inds) {
			m_offspring.emplace_back(*i);
		}
		for (auto &i : m_inds) {
			m_offspring.emplace_back(*i);
		}
	}

	void PopCDGMOEA::updateIdealPoint(int id_pro) {
		int numobj = GET_PRO(id_pro).numObjectives();
		for (int i = 0; i < numobj; ++i) {
			m_ideal[i] = 1.0e14;
			for (int j = 0; j < m_inds.size(); ++j) {
				if (m_inds[j]->objective()[i] < m_ideal[i])
					m_ideal[i] = m_inds[j]->objective()[i];
			}
		}
	}

	void PopCDGMOEA::updateIdealPoint(const std::vector<IndCDGMOEA> &offspring, int id_pro) {
		int numobj = GET_PRO(id_pro).numObjectives();
		for (int i = 0; i < numobj; ++i) {
			for (int j = 0; j < offspring.size(); ++j) {
				if (offspring[j].objective()[i] < m_ideal[i])
					m_ideal[i] = offspring[j].objective()[i];
			}
		}
	}

	void PopCDGMOEA::updateNadirPoint(int id_pro) {
		int numobj = GET_PRO(id_pro).numObjectives();
		for (int i = 0; i < numobj; ++i) {
			m_nadir[i] = -1 * 1.0e14;
			for (int j = 0; j < m_inds.size(); ++j) {
				if (m_nadir[i] < m_inds[j]->objective()[i])
					m_nadir[i] = m_inds[j]->objective()[i];
			}
		}
	}

	void PopCDGMOEA::updateNadirPoint(const std::vector<IndCDGMOEA> &offspring, int id_pro) {  //
		int numobj = GET_PRO(id_pro).numObjectives();
		std::vector<int> SP;
		for (int i = 0; i < offspring.size(); ++i) {
			int flag = 1;
			for (int j = 0; j < numobj; ++j) {
				if (offspring[i].objective()[j] < (m_ideal[j] + m_nadir[j] / 5)) {
					SP.push_back(i);
					break;
				}
			}
		}
		if (SP.size() != 0) {	//find all nondominated solution
			std::vector<IndCDGMOEA> SP1;
			for (int i = 0; i < SP.size(); ++i) {
				SP1.push_back(offspring[SP[i]]);
			}
			nondominatedSorting(SP1, id_pro);
			int size = SP.size(), count = 0;
			std::vector<int>().swap(SP);	//free  SP
			for (int i = 0; i < size; i++) {
				if (SP1[i].rank() == 0) {
					count++;
					SP.push_back(i);
				}
			}
			for (int i = 0; i < numobj; ++i) {
				m_nadir[i] = -1 * 1.0e14;
				for (int j = 0; j < SP.size(); ++j) {
					if (m_nadir[i] < SP1[SP[j]].objective()[i])
						m_nadir[i] = SP1[SP[j]].objective()[i];
				}
			}
		}
	}

	void PopCDGMOEA::gridConstruct(int id_pro) {
		int numobj = GET_PRO(id_pro).numObjectives();
		for (int i = 0; i < numobj; ++i) {
			m_grid_distance[i] = (m_nadir[i] - m_ideal[i]) * (m_grid_div + 1) / (m_grid_div * m_grid_div);
			m_grid_min_max[i].first = m_ideal[i] - (m_nadir[i] - m_ideal[i]) / (2 * m_grid_div);
			m_grid_min_max[i].second = m_nadir[i] + (m_nadir[i] - m_ideal[i]) / (2 * m_grid_div);
		}
	}

	void PopCDGMOEA::assign_GN(int id_pro) {
		int numobj = GET_PRO(id_pro).numObjectives();
		for (int i = 0; i < m_inds.size(); i++) {
			for (int j = 0; j < numobj; j++) {
				m_inds[i]->Gk()[j] = (int)ceil((m_inds[i]->objective(j) - m_grid_min_max[j].first) / m_grid_distance[j]);
			}
		}
		for (int i = 0; i < m_inds.size(); ++i) {
			for (int j = i + 1; j < m_inds.size(); ++j) {
				int GD_max = abs(m_inds[i]->Gk()[0] - m_inds[j]->Gk()[0]);
				for (int k = 0; k < numobj; ++k) {
					if (GD_max < abs(m_inds[i]->Gk()[k] - m_inds[j]->Gk()[k]))
						GD_max = abs(m_inds[i]->Gk()[k] - m_inds[j]->Gk()[k]);
				}
				if (GD_max < m_T || GD_max == m_T) {
					m_inds[i]->GN().push_back(j);
					m_inds[j]->GN().push_back(i);
				}
			}
		}
	}

	void PopCDGMOEA::gridConstruct_assignGN_P_reserve(std::vector<IndCDGMOEA> &offspring, std::vector<int> &P_reserve, int size, int id_pro) {
		int numobj = GET_PRO(id_pro).numObjectives();
		for (int i = 0; i < numobj; ++i) {
			m_grid_distance[i] = (m_nadir[i] - m_ideal[i]) * (m_grid_div + 1) / (m_grid_div * m_grid_div);
			m_grid_min_max[i].first = m_ideal[i] - (m_nadir[i] - m_ideal[i]) / (2 * m_grid_div);
			m_grid_min_max[i].second = m_nadir[i] + (m_nadir[i] - m_ideal[i]) / (2 * m_grid_div);
		}
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < numobj; j++) {
				offspring[P_reserve[i]].Gk()[j] = (int)ceil((offspring[P_reserve[i]].objective()[j] - m_grid_min_max[j].first) / m_grid_distance[j]);
			}
		}
		for (int i = 0; i < size; ++i) {
			for (int j = i + 1; j < size; ++j) {
				int GD_max = abs(offspring[P_reserve[i]].Gk()[0] - offspring[P_reserve[j]].Gk()[0]);
				for (int k = 0; k < numobj; ++k) {
					if (GD_max < abs(offspring[P_reserve[i]].Gk()[k] - offspring[P_reserve[j]].Gk()[k]))
						GD_max = abs(offspring[P_reserve[i]].Gk()[k] - offspring[P_reserve[i]].Gk()[k]);
				}
				if (GD_max < m_T || GD_max == m_T) {
					offspring[P_reserve[i]].GN().push_back(P_reserve[j]);
					offspring[P_reserve[j]].GN().push_back(P_reserve[i]);
				}
			}
		}

	}

	void PopCDGMOEA::assign_S(std::vector<IndCDGMOEA> &offspring, std::vector<int> P_reserve, int id_pro) {
		int numobj = GET_PRO(id_pro).numObjectives();
		int size = pow(m_grid_div, numobj - 1);
		if (numobj == 2) {
			for (int i = 0; i < P_reserve.size(); ++i) {
				for (int l = 0; l < numobj; ++l) {
					for (int j = 0; j < numobj; ++j) {
						if (j != l) {
							for (int k = 0; k < m_grid_div; ++k) {	//M^(K-1)							
								if (offspring[P_reserve[i]].Gk()[j] == k + 1)
									m_S[l][k].push_back(P_reserve[i]);
							}
						}
					}
				}
			}
		}
		else if (numobj == 3) {
			for (int i = 0; i < P_reserve.size(); ++i) {
				for (int l = 0; l < numobj; ++l) {
					if (l == 0) {
						for (int k = 0; k < m_grid_div; ++k) {	//M^(K-1)
							for (int t = 0; t < m_grid_div; ++t) {
								if (offspring[P_reserve[i]].Gk()[1] == t + 1 && offspring[P_reserve[i]].Gk()[2] == k + 1)
									m_S[l][m_grid_div * k + t].push_back(P_reserve[i]);
							}
						}
					}
					else if (l == 1) {
						for (int k = 0; k < m_grid_div; ++k) {	//M^(K-1)
							for (int t = 0; t < m_grid_div; ++t) {
								if (offspring[P_reserve[i]].Gk()[0] == t + 1 && offspring[P_reserve[i]].Gk()[2] == k + 1)
									m_S[l][m_grid_div * k + t].push_back(P_reserve[i]);
							}
						}
					}
					else {
						for (int k = 0; k < m_grid_div; ++k) {	//M^(K-1)
							for (int t = 0; t < m_grid_div; ++t) {
								if (offspring[P_reserve[i]].Gk()[0] == t + 1 && offspring[P_reserve[i]].Gk()[1] == k + 1)
									m_S[l][m_grid_div * k + t].push_back(P_reserve[i]);
							}
						}
					}
				}
			}
		}
		/*test m_S*/
		for (int l = 0; l < numobj; ++l) {
			int num = 0;
			for (int k = 0; k < size; ++k) {
				num += m_S[l][k].size();
			}
			if (numobj == 2 && num != P_reserve.size())
				std::cout << "assign m_S is error:num=" << num << std::endl;
		}

	}

	void PopCDGMOEA::RBS(std::vector<IndCDGMOEA> &offspring, std::vector<int> P_reserve, int id_pro) {
		int numobj = GET_PRO(id_pro).numObjectives();
		std::vector<std::vector<int>> R_(P_reserve.size(), std::vector<int>(numobj));
		/*Decomposition_based Ranking*/
		for (int l = 0; l < numobj; ++l) {
			for (int k = 0; k < pow(m_grid_div, numobj - 1); ++k) {
				std::vector<std::pair<double, int>> v;	//first store objective value, second store index
				for (int j = 0; j < m_S[l][k].size(); ++j) {
					v.push_back(std::make_pair(offspring[m_S[l][k][j]].objective()[l], m_S[l][k][j]));
				}
				if (v.size()) {
					sortl(v);	//sort of objective value according to the ascending
					for (int i = 0; i < m_S[l][k].size(); ++i) {
						offspring[v[i].second].R()[l] = i + 1;
					}
				}
			}
		}
		/*Lexicographic Sorting*/
		for (int i = 0; i < P_reserve.size(); ++i) {
			m_R.push_back(offspring[P_reserve[i]].R());
			sortR(m_R[i], R_[i]);
		}
		std::vector<int> number;	//to get index of P_reserve in offspring
		for (int i = 0; i < P_reserve.size(); ++i) {
			number.push_back(i);
		}
		sortL(R_, number, id_pro);
		for (int i = 0; i < m_inds.size(); ++i) {
			*m_inds[i] = offspring[P_reserve[number[i]]];
		}
		/*free the memory*/
		std::vector<std::vector<int>>().swap(m_R);
		for (int l = 0; l < numobj; ++l) {
			for (int k = 0; k < pow(m_grid_div, numobj - 1); ++k)
				if (m_S[l][k].size() != 0)
					std::vector<int>().swap(m_S[l][k]);	//clear m_S	
		}

		/*for (int i = 0; i < offspring.size(); ++i) {
			std::vector<int>().swap(offspring[i].GN());
		}*/

	}

	void PopCDGMOEA::sortl(std::vector<std::pair<double, int>> &v) {	//selection sort
		for (int i = 0; i < v.size(); ++i) {
			int min = i;
			for (int j = i + 1; j < v.size(); ++j) {
				if (v[j].first < v[min].first)
					min = j;
			}
			auto temp = v[i];
			v[i] = v[min];
			v[min] = temp;
		}
	}

	void PopCDGMOEA::sortR(const std::vector<int> &R, std::vector<int> &R_) {
		for (int i = 0; i < R.size(); ++i)
			R_[i] = R[i];
		for (int i = 0; i < R_.size(); ++i) {
			int min = i;
			for (int j = i + 1; j < R_.size(); ++j) {
				if (R_[j] < R_[min])
					min = j;
			}
			auto temp = R_[i];
			R_[i] = R_[min];
			R_[min] = temp;
		}
	}

	void PopCDGMOEA::sortL(std::vector<std::vector<int>> &R_, std::vector<int> &number, int id_pro) {
		int numobj = GET_PRO(id_pro).numObjectives();
		for (int l = 0; l < numobj; ++l) {
			for (int i = 0; i < R_.size(); ++i) {
				int min = i;
				for (int j = i + 1; j < R_.size(); ++j) {
					if (l == 0 && R_[j][l] < R_[min][l])
						min = j;
					else if (l == 1 && R_[j][0] == R_[min][0] && R_[j][l] < R_[min][l])
						min = j;
					else if (l == 2 && R_[j][0] == R_[min][0] && R_[j][1] == R_[min][1] && R_[j][l] < R_[min][l])
						min = j;
				}
				if (i != min) {
					auto temp1 = R_[i];
					R_[i] = R_[min];
					R_[min] = temp1;
					int temp2 = number[i];
					number[i] = number[min];
					number[min] = temp2;
				}
			}
		}
	}

	void PopCDGMOEA::selectRandom(int number, std::vector<int> &result, int id_rnd) {
		std::vector<int> candidate;
		for (int i = 0; i < m_inds.size(); ++i) {
			candidate.push_back(i);
		}
		result.resize(number);
		for (int i = 0; i < number; ++i) {
			int idx = GET_RND(id_rnd).uniform.nextNonStd(0, (int)candidate.size() - i);
			result[i] = candidate[idx];
			if (idx != candidate.size() - (i + 1)) candidate[idx] = candidate[candidate.size() - (i + 1)];
		}
	}

	void PopCDGMOEA::nondominatedSorting(std::vector<IndCDGMOEA> &offspring, int id_pro) {
		std::vector<std::vector<Real> *> objs;
		for (auto &i : offspring)
			objs.emplace_back(&i.objective());
		std::vector<int> rank;
		NS::fast_sort<Real>(objs, rank, GET_PRO(id_pro).optMode());
		for (size_t i = 0; i < offspring.size(); ++i)
			offspring[i].setRank(rank[i]);
	}

	int PopCDGMOEA::evolve(int id_pro, int id_alg, int id_rnd) {
		int tag = kNormalEval;
		//std::cout << "the " << m_iter << " generation" << std::endl;
		/*step2:reproduction*/
		tag = evolveMO(id_pro, id_alg, id_rnd);
		/*step3:update of the ideal and nadir points*/
		updateIdealPoint(m_offspring, id_pro);
		if ((m_iter % 10) == 0 && m_iter != 0)
			updateNadirPoint(m_offspring, id_pro);	//updated nadir point every 50 generations
		/*step4:update of the grid system*/
		evalEens(id_pro, id_rnd);
		/*record objective every generation*/
		//record_x();
		//record_x_offspring();
		//record_f();
		//record_f_offspring();

		////下面测试输出结果
		//for (const auto& i : m_inds) {
		//	std::cout << i->objective()[0] << "    " << i->objective()[1] << std::endl;
		//}
		//std::cout << "* * * * * * * * * * * * * * *" << std::endl;
		//std::cout << "第" << "  " << m_iter << "  代" << std::endl;
		//std::cout << "* * * * * * * * * * * * * * *" << std::endl;

		m_iter++;
		return tag;
	}

	int PopCDGMOEA::evolveMO(int id_pro, int id_alg, int id_rnd) {		//step2:reproduction	
		int tag = kNormalEval;
		//assign NS of individual in P
		if (m_iter) {
			for (size_t i = 0; i < m_inds.size(); ++i) {
				std::vector<size_t>().swap(m_inds[i]->GN());
			}
			assign_GN(id_pro);
		}
		int m = 0;
		for (int i = 0; i < m_inds.size(); ++i) {
			double rand = GET_RND(id_rnd).uniform.next();
			std::vector<size_t> result(3);
			if (rand < m_delta && m_inds[i]->GN().size()>3)
				selectInNeighborhood(3, m_inds[i]->GN(), result, id_rnd);
			else
				select(i, 3, result, id_rnd);
			/*adopt DE operator in MOEA/D*/
			m_offspring[m++] = *m_inds[i];
			crossMutate(result, m_offspring[m], id_pro, id_rnd);
			tag = m_offspring[m].evaluate(id_pro, id_alg);
			if (tag != kNormalEval) break;
			m++;
		}
		return tag;
	}

	void PopCDGMOEA::evalEens(int id_pro, int id_rnd) {	//step5:rank_based selection
		//delete individual of objective>m_nadir	
		std::vector<int> P_delete, P_reserve;
		for (int i = 0; i < m_offspring.size(); ++i) {
			int flag = 0;
			for (int j = 0; j < GET_PRO(id_pro).numObjectives(); ++j) {
				if (m_nadir[j] < m_offspring[i].objective()[j])
					flag = 1;
			}
			if (flag)
				P_delete.push_back(i);
			else
				P_reserve.push_back(i);
		}
		if ((P_delete.size() + P_reserve.size()) != m_offspring.size())
			std::cout << "step3 is error" << std::endl;

		if (P_reserve.size() != 0)
			gridConstruct_assignGN_P_reserve(m_offspring, P_reserve, P_reserve.size(), id_pro);
		else
			//	std::cout << "the " << m_iter << " generation : " << "reserve = 0" << std::endl;

			if (P_reserve.size() < m_inds.size()) {	// when Preserve < N
				for (int i = 0; i < P_reserve.size(); ++i) {
					*m_inds[i] = m_offspring[P_reserve[i]];
				}
				//select (N-P_reserve) individual from P_delete randomly
				std::vector<int> result(m_inds.size() - P_reserve.size());
				selectRandom(m_inds.size() - P_reserve.size(), result, id_rnd);
				int m = 0;
				for (int i = P_reserve.size(); i < m_inds.size(); ++i) {
					*m_inds[i] = m_offspring[P_delete[result[m++]]];
					//m_inds[i]->solut() = m_offspring[P_delete[result[m++]]].solut();
				}
			}
			else {	// when Preserve > N , /*step5:rank_based selection*/ RBS
				assign_S(m_offspring, P_reserve, id_pro);
				RBS(m_offspring, P_reserve, id_pro);
			}
	}

	/*void PopCDGMOEA::record_x() {
		int n = global::ms_global->m_problem->variable_size();
		if (m_iter == 1) {
			std::ofstream out;
			out.open("./result/CDG_MOEA_var.txt");
			if (out && n == 2) {
				out << "the  " << m_iter << "  generation" << std::endl;
				out << std::setw(10) << "x1" << std::setw(10) << "x2" << std::endl;
				for (int i = 0; i < m_inds.size(); ++i) {
					for (int j = 0; j < n; ++j) {
						out << std::setw(10) << m_inds[i]->variable()[j];
					}
					out << std::endl;
				}
			}
			else if (out && n == 3) {
				out << "the  " << m_iter << "  generation" << std::endl;
				out << std::setw(10) << "x1" << std::setw(10) << "x2" << std::setw(10) << "x3" << std::endl;
				for (int i = 0; i < m_inds.size(); ++i) {
					for (int j = 0; j < n; ++j) {
						out << std::setw(10) << m_inds[i]->variable()[j];
					}
					out << std::endl;
				}
			}
			out.close();
		}
		else {
			std::ofstream out;
			out.open("./result/CDG_MOEA_var.txt", std::ios::app);
			if (out && n == 2) {
				out << std::endl << "the  " << m_iter << "  generation" << std::endl;
				out << std::setw(10) << "x1" << std::setw(10) << "x2" << std::endl;
				for (int i = 0; i < m_inds.size(); ++i) {
					for (int j = 0; j < n; ++j) {
						out << std::setw(10) << m_inds[i]->variable()[j];
					}
					out << std::endl;
				}
			}
			else if (out && n == 3) {
				out << std::endl << "the  " << m_iter << "  generation" << std::endl;
				out << std::setw(10) << "x1" << std::setw(10) << "x2" << std::setw(10) << "x3" << std::endl;
				for (int i = 0; i < m_inds.size(); ++i) {
					for (int j = 0; j < n; ++j) {
						out << std::setw(10) << m_inds[i]->variable()[j];
					}
					out << std::endl;
				}
			}
			out.close();
		}

	}

	void PopCDGMOEA::record_x_offspring() {
		int n = global::ms_global->m_problem->variable_size();
		if (m_iter == 1) {
			std::ofstream out;
			out.open("./result/CDG_MOEA_var_offspring.txt");
			if (out && n == 2) {
				out << "the  " << m_iter << "  generation" << std::endl;
				out << std::setw(10) << "x1" << std::setw(10) << "x2" << std::endl;
				for (int i = 0; i < m_offspring.size(); ++i) {
					for (int j = 0; j < n; ++j) {
						out << std::setw(10) << m_offspring[i].variable()[j];
					}
					out << std::endl;
				}
			}
			out.close();
		}
		else {
			std::ofstream out;
			out.open("./result/CDG_MOEA_var_offspring.txt", std::ios::app);
			if (out && n == 2) {
				out << std::endl << "the  " << m_iter << "  generation" << std::endl;
				out << std::setw(10) << "x1" << std::setw(10) << "x2" << std::endl;
				for (int i = 0; i < m_offspring.size(); ++i) {
					for (int j = 0; j < n; ++j) {
						out << std::setw(10) << m_offspring[i].variable()[j];
					}
					out << std::endl;
				}
			}
			out.close();
		}

	}

	void PopCDGMOEA::record_f() {
		int m = global::ms_global->m_problem->objective_size();
		if (m_iter == 1) {
			std::ofstream out;
			out.open("./result/CDG_MOEA_obj.txt");
			if (out && m == 2) {
				out << "the  " << m_iter << "  generation" << std::endl;
				out << "f1\t" << "f2" << std::endl;
				for (int i = 0; i < m_inds.size(); ++i) {
					for (int j = 0; j < m; ++j) {
						out << "\t" << m_inds[i]->objective()[j];
					}
					out << std::endl;
				}
			}
			else if (out && m == 3) {
				out << "the  " << m_iter << "  generation" << std::endl;
				out << std::setw(10) << "f1" << std::setw(10) << "f2" << std::setw(10) << "f3" << std::endl;
				for (int i = 0; i < m_inds.size(); ++i) {
					for (int j = 0; j < m; ++j) {
						out << std::setw(10) << m_inds[i]->objective()[j];
					}
					out << std::endl;
				}
			}
			out.close();
		}
		else {
			std::ofstream out;
			out.open("./result/CDG_MOEA_obj.txt", std::ios::app);
			if (out && m == 2) {
				out << std::endl << "the  " << m_iter << "  generation" << std::endl;
				out << std::setw(10) << "f1" << std::setw(10) << "f2" << std::endl;
				for (int i = 0; i < m_inds.size(); ++i) {
					for (int j = 0; j < m; ++j) {
						out << std::setw(10) << m_inds[i]->objective()[j];
					}
					out << std::endl;
				}
			}
			else if (out && m == 3) {
				out << std::endl << "the  " << m_iter << "  generation" << std::endl;
				out << std::setw(10) << "f1" << std::setw(10) << "f2" << std::setw(10) << "f3" << std::endl;
				for (int i = 0; i < m_inds.size(); ++i) {
					for (int j = 0; j < m; ++j) {
						out << std::setw(10) << m_inds[i]->objective()[j];
					}
					out << std::endl;
				}
			}
			out.close();
		}
	}

	void PopCDGMOEA::record_f_offspring() {
		int m = global::ms_global->m_problem->objective_size();
		if (m_iter == 1) {
			std::ofstream out;
			out.open("./result/CDG_MOEA_obj_offspring.txt");
			if (out && m == 2) {
				out << "the  " << m_iter << "  generation" << std::endl;
				out << std::setw(10) << "f1" << std::setw(10) << "f2" << std::endl;
				for (int i = 0; i < m_offspring.size(); ++i) {
					for (int j = 0; j < m; ++j) {
						out << std::setw(10) << m_offspring[i].objective()[j];
					}
					out << std::endl;
				}
			}
			out.close();
		}
		else {
			std::ofstream out;
			out.open("./result/CDG_MOEA_obj_offspring.txt", std::ios::app);
			if (out && m == 2) {
				out << std::endl << "the  " << m_iter << "  generation" << std::endl;
				out << std::setw(10) << "f1" << std::setw(10) << "f2" << std::endl;
				for (int i = 0; i < m_offspring.size(); ++i) {
					for (int j = 0; j < m; ++j) {
						out << std::setw(10) << m_offspring[i].objective()[j];
					}
					out << std::endl;
				}
			}
			out.close();
		}

	}*/
}
