#include "csiwdn_individual.h"
#include "../../../problem/realworld/csiwdn/csiwdn.h"

namespace ofec {
	IndCSIWDN& IndCSIWDN::operator=(IndCSIWDN& indi) {
		Individual::operator=(indi);
		m_pv = indi.m_pv;
		m_pu = indi.m_pu;

		return *this;
	}

	IndCSIWDN& IndCSIWDN::operator=(IndCSIWDN&& indi) {
		Individual::operator=(std::move(indi));
		m_pv = std::move(indi.m_pv);
		m_pu = std::move(indi.m_pu);

		return *this;
	}

	int IndCSIWDN::sourceIdx(int id_pro) {
		int z = 0;
		while ((z + 1) < GET_CSIWDN(id_pro).numberSource() && GET_CSIWDN(id_pro).phase() >= ((m_pu.variable().startTime(z + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
			z++;
		}
		return z;
	}
	void IndCSIWDN::mutateFirstPart(const std::vector<std::vector<Real>>& pro,int id_pro,int id_rnd) {
		int z = sourceIdx(id_pro);
		int q = 0;
		//int n = GET_CSIWDN(id_pro).phase() / GET_CSIWDN(id_pro).evalPhase();
		//while ((q + 1) < GET_CSIWDN(id_pro).numberSource() && (n * GET_CSIWDN(id_pro).evalPhase()) >= ((m_pu.variable().startTime(q + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
		//	q++;
		//}
		m_pu.variable() = m_var;

		mutateLocation(id_pro,id_rnd,pro);
	}

	void  IndCSIWDN::mutateSecondPart(int id_pro, Real F, int jdx, const std::vector<std::vector<Real>> & pro,
		Solution<VarCSIWDN> *r1,
		Solution<VarCSIWDN> *r2,
		Solution<VarCSIWDN> *r3,
		Solution<VarCSIWDN> *r4,
		Solution<VarCSIWDN> *r5) {
	
		m_pv.variable() = m_var;

		mutateMass(id_pro,F, jdx, r1, r2, r3, r4, r5);
	}

	void  IndCSIWDN::mutateMass(int id_pro, Real F, int jdx, Solution<VarCSIWDN> *r1,
		Solution<VarCSIWDN> *r2,
		Solution<VarCSIWDN> *r3,
		Solution<VarCSIWDN> *r4,
		Solution<VarCSIWDN> *r5) {

		/// mutate multiplier
		long u_multi =  GET_CSIWDN(id_pro).maxMultiplier();
		long l_multi =  GET_CSIWDN(id_pro).minMultiplier();

		
		size_t pv_size = m_pv.variable().duration(jdx) /  GET_CSIWDN(id_pro).patternStep();
		if (m_pv.variable().duration(jdx) %  GET_CSIWDN(id_pro).patternStep() != 0)
			++pv_size;
		long time_step =  GET_CSIWDN(id_pro).patternStep();
		std::vector<Real> vec_r1(r1->variable().multiplier(jdx));
		std::vector<Real> vec_r2(r2->variable().multiplier(jdx));
		std::vector<Real> vec_r3(r3->variable().multiplier(jdx));
		vec_r1.resize(pv_size);
		vec_r2.resize(pv_size);
		vec_r3.resize(pv_size);
		

		std::vector<Real> vec_r4;
		std::vector<Real> vec_r5;
		if (r4&&r5) {
			vec_r4 = r4->variable().multiplier(jdx);
			vec_r5 = r5->variable().multiplier(jdx);
			vec_r4.resize(pv_size);
			vec_r5.resize(pv_size);
		}
		float temp;
		for (size_t i = 0; i < pv_size; ++i) {
			m_pv.variable().multiplier(jdx)[i] = (vec_r1[i]) + F * ((vec_r2[i]) - (vec_r3[i]));
			if (r4 && r5) m_pv.variable().multiplier(jdx)[i] += F * ((vec_r4[i]) - (vec_r5[i]));
			if ((m_pv.variable().multiplier(jdx)[i]) > u_multi) {
				m_pv.variable().multiplier(jdx)[i] = ((vec_r1[i]) + u_multi) / 2;
			}
			else if ((m_pv.variable().multiplier(jdx)[i]) < l_multi) {
				m_pv.variable().multiplier(jdx)[i] = ((vec_r1[i]) + l_multi) / 2;
			}
		}

	}

	void  IndCSIWDN::mutateLocation(int id_pro,int id_rnd,const std::vector<std::vector<Real>>& pro) {

		std::vector<Real> single_roulette_node(pro[0].size() + 1, 0.);    // roulette wheel selection for node location
		std::vector<std::vector<Real>> roulette_node(GET_CSIWDN(id_pro).numberSource(), single_roulette_node);
		int z = sourceIdx(id_pro), q = 0;
		//int n = GET_CSIWDN(id_pro).phase() / GET_CSIWDN(id_pro).evalPhase();
		//while ((q + 1) < GET_CSIWDN(id_pro).numberSource() && (n * GET_CSIWDN(id_pro).evalPhase()) >= ((m_pu.variable().startTime(q + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
		//	q++;
		//}
		for (int j = q; j <= z; ++j) {
			for (size_t i = 0; i < pro[j].size(); ++i) {
				roulette_node[j][i + 1] = roulette_node[j][i] + pro[j][i];
			}
		}
		std::vector<Real> p;
		p.resize(roulette_node.size(), 0.0);
		for (int j = q; j <= z; ++j) {
			p[j] = GET_RND(id_rnd).uniform.nextNonStd<Real>(0, roulette_node[j][roulette_node[j].size() - 1]);
		}

		for (int j = q; j <= z; ++j) {
			for (size_t i = 0; i < roulette_node[j].size() - 1; ++i)
				if (p[j] >= roulette_node[j][i] && p[j] < roulette_node[j][i + 1])
					m_pu.variable().index(j) = i + 1;
		}
	}

	void  IndCSIWDN::recombine(int id_pro, int id_rnd, Real CR) {
		m_pu = m_pv;
		int z = sourceIdx(id_pro);

		size_t pv_size_multiplier = m_pv.variable().multiplier(z).size();
		std::vector<Real> mult_temp(variable().multiplier(z));
		mult_temp.push_back(variable().startTime(z));
		++pv_size_multiplier;
		mult_temp.resize(pv_size_multiplier);

		size_t I = GET_RND(id_rnd).uniform.nextNonStd<size_t>(0, pv_size_multiplier);
		for (size_t i = 0; i < pv_size_multiplier; ++i) {
			Real p = GET_RND(id_rnd).uniform.next();
			if (i == pv_size_multiplier - 1) {
				if (p <= CR || i == I) {
					m_pu.variable().startTime(z) = m_pv.variable().startTime(z);
				}
				else {
					m_pu.variable().startTime(z) = mult_temp[i];
				}
			}
			else {
				if (p <= CR || i == I) {
					m_pu.variable().multiplier(z)[i] = m_pv.variable().multiplier(z)[i];
				}
				else {
					m_pu.variable().multiplier(z)[i] = mult_temp[i];
				}
			}
		}

	}

	int  IndCSIWDN::select(int id_pro, int id_alg, bool is_stable) {

		int tag = m_pu.evaluate(id_pro, id_alg);

		if ((is_stable && (m_pu_distance_fitness > m_distance_fitness)) || (!is_stable && m_pu.dominate(m_obj, GET_PRO(id_pro).optMode()))) {
			m_var = m_pu.variable();
			m_obj = m_pu.objective();
			m_con = m_pu.constraint();
			m_improved = true;
		}
		else {
			m_improved = false;
		}
		return tag;
	}

	bool  IndCSIWDN::sameLocation(int id_pro, IndCSIWDN & indi) {
		int z = sourceIdx(id_pro);
		return m_var.index(z) == indi.variable().index(z);
	}

	void  IndCSIWDN::initialize(int id, int id_pro, int id_rnd ) {
		int z = GET_CSIWDN(id_pro).endSourceIdx(), q = GET_CSIWDN(id_pro).startSourceIdx();
		if (!GET_CSIWDN(id_pro).isAlgorithmStarted()) {
			Individual::initialize(id, id_pro, id_rnd);
		}
		else {
			for (size_t j = 0; j < GET_CSIWDN(id_pro).numberSource(); j++) {
				m_var.flagLocation(j) = false;
				if (GET_CSIWDN(id_pro).initType() == InitType::AHC) {
					int size_clusters = GET_CSIWDN(id_pro).get_Q_AHC().cluster_size();
					int random_identifier;
					int size;
					do {
						random_identifier =  GET_RND(id_rnd).uniform.nextNonStd<int>(0, size_clusters);
						size = GET_CSIWDN(id_pro).get_Q_AHC().clusters()[random_identifier].member.size();
					} while (size <= 0);
					int random =  GET_RND(id_rnd).uniform.nextNonStd<int>(0, size);
					m_var.index(j) = GET_CSIWDN(id_pro).get_Q_AHC().clusters()[random_identifier].member[random] + 1;
				}
				else if (GET_CSIWDN(id_pro).initType() == InitType::Random) {
					m_var.index(j) =  GET_RND(id_rnd).uniform.nextNonStd<int>(1, GET_CSIWDN(id_pro).numberNode() + 1);
				}
				else if (GET_CSIWDN(id_pro).initType() == InitType::BeVisited) {
					int random_identifier, size, size_clusters = GET_CSIWDN(id_pro).get_Q_AHC().clusters().size();
					if (j >= q && j <= z) {
						do {
							random_identifier =  GET_RND(id_rnd).uniform.nextNonStd<int>(0, size_clusters);
							size = GET_CSIWDN(id_pro).get_Q_AHC().clusters()[random_identifier].member.size();
						} while (size <= 0);
						std::vector<Real> roulette(size + 1, 0);
						for (size_t i = 0; i < roulette.size() - 1; ++i) {
							roulette[i + 1] = roulette[i] + GET_CSIWDN(id_pro).getProBeVisited()[j][GET_CSIWDN(id_pro).get_Q_AHC().clusters()[random_identifier].member[i]];
						}
						Real random_value =  GET_RND(id_rnd).uniform.nextNonStd<Real>(roulette[0], roulette[roulette.size() - 1]);
						for (size_t i = 0; i < roulette.size() - 1; ++i) {
							if (random_value >= roulette[i] && random_value < roulette[i + 1]) {
								m_var.index(j) = GET_CSIWDN(id_pro).get_Q_AHC().clusters()[random_identifier].member[i] + 1;
							}
						}

					}
				}
				m_var.source(j) = 1.0;
				m_var.startTime(j) = GET_CSIWDN(id_pro).getOptima().variable(0).startTime(j);
				m_var.duration(j) = GET_CSIWDN(id_pro).getOptima().variable(0).duration(j);
				size_t size = m_var.duration(j) / GET_CSIWDN(id_pro).patternStep();
				if (m_var.duration(j) % GET_CSIWDN(id_pro).patternStep() != 0) ++size;
				m_var.multiplier(j).resize(size);
				for (auto& j : m_var.multiplier(j)) {
					j = GET_RND(id_rnd).uniform.nextNonStd<Real>(GET_CSIWDN(id_pro).minMultiplier(), GET_CSIWDN(id_pro).maxMultiplier());
				}
				m_pu.variable() = variable();
				m_pu.objective() = objective();
				m_pu.constraint() = constraint();

				m_pv.variable() = variable();
				m_pv.objective() = objective();
				m_pv.constraint() = constraint();
			}
		}
	}

	Solution< VarCSIWDN> &  IndCSIWDN::trial() {
		return m_pu;
	}

	void  IndCSIWDN::coverFirstPart(VarCSIWDN& indi, int id_pro) {
		int z = 0, i = 0;
		//int k = 0;
		//z = GET_CSIWDN(id_pro).endSourceIdx();
		//k = GET_CSIWDN(id_pro).phase() / GET_CSIWDN(id_pro).evalPhase();
		//while ((i + 1) < GET_CSIWDN(id_pro).numberSource() && (k * GET_CSIWDN(id_pro).evalPhase()) >= ((indi.startTime(z + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
		//	i++;
		//}
		for (; i <= z; i++) {
			std::strcpy(m_var.location(i), indi.location(i));
			m_var.index(i) = indi.index(i);
		}
	}

	void  IndCSIWDN::coverSecondPart(VarCSIWDN& indi, int id_pro) {
		int z = 0, i = 0;
		//int k = 0;
		//z = GET_CSIWDN(id_pro).endSourceIdx();
		//k = GET_CSIWDN(id_pro).phase() / GET_CSIWDN(id_pro).evalPhase();
		//while ((i + 1) < GET_CSIWDN(id_pro).numberSource() && (k * GET_CSIWDN(id_pro).evalPhase()) >= ((indi.startTime(z + 1) / GET_CSIWDN(id_pro).intervalTimeStep()))) {
		//	i++;
		//}
		for (; i <= z; i++) {
			m_var.startTime(i) = indi.startTime(i);
			m_var.duration(i) = indi.duration(i);
			m_var.multiplier(i) = indi.multiplier(i);
		}
	}
}