#ifndef MP_POP_GL_COM_H
#define MP_POP_GL_COM_H
#include "../../../../../utility/functional.h"
#include "../gl_pop_seq.h"
#include "../../../template/combination/multi_population/mp_com_pop.h"
#include "../../../template/combination/sequence/sequence_algorithm.h"
namespace ofec {
	template<typename TAdaptor>
	class PopGLMpCom : public MP_COM_Pop<PopGLSeq<TAdaptor>> {
	public:
		using AdaptorType = typename TAdaptor;
		using OpType = typename TAdaptor::OpType;
		using IndividualType = typename TAdaptor::IndividualType;
		using InterpreterType = typename TAdaptor::InterpreterType;

	protected:
		
	public:
		void setRadius(Real radius) {
			PopGLSeq<TAdaptor>::setRadius(radius);
			m_innerRadius = radius;
		}
		virtual void initPopAround(int id_pro, int id_alg, const IndividualType& center, Real radius, size_t* popSize = nullptr) override;
	};
	template<typename TAdaptor>
	void PopGLMpCom<TAdaptor>::initPopAround(int id_pro, int id_alg, const IndividualType& center, Real radius, size_t* popSize){
		if (popSize != nullptr) {
			resize(*popSize, id_pro);
		}
		for (int id(0); id < m_inds.size(); ++id) {
			m_inds[id]->setId(id);
		}
		m_his.clear();
		setRadius(radius);
		auto& op =GET_ASeq(id_alg).getOp<OpType>();
		auto& interperter = GET_ASeq(id_alg).getInterpreter<InterpreterType>();
		*m_inds.front() = center;
		auto id_rnd = GET_ALG(id_alg).idRandom();
		for (int id(1); id < m_inds.size(); ++id) {
			m_inds[id]->initialize(id, id_pro, id_rnd);
			op.learn_from_other(*m_inds[id], id_rnd, id_pro, interperter, center);
		}
		for (auto& it : m_inds) {
			interperter.stepFinal(id_pro, *it);
			op.evaluate(id_pro, id_alg, id_rnd, *it);
		}
		initializeMemory(id_pro, id_alg);
		m_offspring.clear();
		for (int i = 0; i < this->size(); i++) {
			m_offspring.push_back(*this->m_inds[i]);
		}

	}

}


#endif