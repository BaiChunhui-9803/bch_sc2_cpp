//Register CSIWDN "CSIWDN" CSIWDN

#ifndef OFEC_CSIWDN_H
#define OFEC_CSIWDN_H                                                          

#include "../../../../core/problem/optima.h"
#include "../../../../core/problem/problem.h"
#include "../../../../core/problem/solution.h"
#include "csiwdn_encoding.h"
#include <string>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sstream>
#include <fstream>
#include <exception>

#include "epanet/epanet.h"
#include "q_ahc.h"
#include <mutex>

#include "../../../../utility/machine_learning/libtorch/model/lstm.h"

#define GET_CSIWDN(id_pro) dynamic_cast<CSIWDN&>(GET_PRO(id_pro))

namespace ofec {
	enum class InitType { Random, Distance, AHC, BeVisited };

	/* Contamination Source Identification for Water Distribution Network */
	class CSIWDN : public Problem, public epanet::EPANET {
	public:
		struct Pipe {
			std::string pipe_id;
			std::string sNode;
			std::string eNode;
			double length;
			double diameter;
			std::vector<float> flow_rate;
		};

		struct Pipe_node {
			int m_node;
			double m_weight;
		};

		struct PathInfo {
			int m_start, m_end;
			double m_length;
			friend bool operator<(const PathInfo &a, const PathInfo &b) {
				return a.m_length > b.m_length;
			}
		};

	protected:
		static std::mutex ms_mutex_epanet;

		int m_phase = 1;
		int m_idx_source = 0;
		Optima<VarCSIWDN> m_optima;
		VarCSIWDN m_real_PS_read;
		bool m_is_detected = false;

		std::vector<std::vector<float>> m_Ciobs;	// Real value
		int m_num_source;							// number of source    
		int m_num_sensor;							// number of sensors
		int m_num_node;								 // number of nodes
		long m_quality_time_step;					// time Step of recording quality 
		int m_num_quality_records_each_phase;		// number of quality records of each phase
		int m_total_phase;							// number of phases (evaluation function changes when phase increases)
		int m_first_phase;                          // the phase when contamination detected by any sensor
	
		long m_min_duration, m_max_duration;
		long m_total_duration;
		float m_min_multiplier, m_max_multiplier;
		std::vector<long> m_inject_duration;

		long m_min_start_time, m_max_start_time;

		/* for simplification of problem */
		long m_pattern_step;
		int m_num_pattern;   // number of pattern : m_total_duration / m_pattern_step

		std::vector<std::string> m_sensor_Loc;          // location of sensors , first time detected
		std::string m_file_name;                // file name of input data  
		std::stringstream m_map_inp;
		std::stringstream m_map_rpt;
		std::stringstream m_hyd;

		std::vector<int> m_pattern_index;
		InitType m_init_type = InitType::AHC;

		std::vector<bool> m_index_flag;
		bool m_start_algorithm;

		std::vector<Real> m_distance_node;  //compute distance vaule of nodes, for initialize by distance 

		std::vector<std::string> m_node_name;
		std::map<std::string, int> m_node_id;
		std::vector<std::vector<Real>> m_location;
		std::vector<std::vector<bool>> m_adjmatrix;

		size_t m_pop_identifier;
		Q_AHC m_clusters;

		std::vector<std::vector<int>> m_num_be_visited;
		std::vector<std::vector<double>> m_pro_be_visited;

		std::vector<Pipe> m_pipe;
		std::vector<std::vector<float>> m_node_quality;

		/* Libtorch prediction */
		bool m_use_lstm_model;
		std::unique_ptr<LstmLinear> m_lstm_model;
		bool m_lstm_model_initialized;
		std::vector<std::pair<float, float>> m_input_range, m_output_range;
		int m_input_size;
		int m_output_size;
		int m_hidden_size;
		int m_num_layers;
		std::string m_model_file_name;

	protected:
		void initialize_() override;
		void initSolution(SolBase &s, int id_rnd) const override;
		void evaluate_(SolBase &s, bool effective) override;
		void evaluateObjective(VarCSIWDN &x, std::vector<Real> &obj);
		void readParameter();
		void setPhase(int num) { m_phase = num;}
		void readLocation();
		void initializeEPANET();
		void updateAdjmatrix();
		void readConnect();
		/* Libtorch prediction */
		void initializeLSTM();
		void getCitByLSTM(VarCSIWDN &sol, std::vector<std::vector<float>> &data);

	public:
		~CSIWDN();
		bool same(const SolBase &s1, const SolBase &s2) const override { return true; } //TODO
		Real variableDistance(const SolBase &s1, const SolBase &s2) const override { return 0.0; }  //TODO
		Real variableDistance(const VarBase &s1, const VarBase &s2) const override { return 0.0; }  //TODO
		const Optima<VarCSIWDN> &getOptima() const { return m_optima; }
		void getCitByEPANET(VarCSIWDN &sol, std::vector<std::vector<float>> &data);

		size_t popIdentifier() const { return  m_pop_identifier; }
		void setPopIdentifier(size_t i) { m_pop_identifier = i; }
		int phase() const { return m_phase; }
		bool isDetected()const { return m_is_detected; }
		void phaseNext();
		int idxSource() const { return m_idx_source; }
		int firstPhase() { return m_first_phase; }
		int interval() const { return m_num_quality_records_each_phase; }
		int totalPhase() const { return m_total_phase; }
		long totalRunTime() const { return m_total_duration; }
		const std::vector<Real> &distanceNode() const { return m_distance_node; }
		std::vector<Real> &distanceNode() { return m_distance_node; }
		int numSensor() const { return m_num_sensor; }
		int numSource() const { return m_num_source; }
		const std::vector<std::vector<float>> &observationConcentration() const { return m_Ciobs; }
		const std::vector<std::vector<float>> &nodeQuality() const { return m_node_quality; }
		const std::map<std::string, int> &nodeId() const { return m_node_id; }
		bool isTimeOut() const;
		int startSourceIdx() const;
		int endSourceIdx() const;
		int numberNode() const { return m_num_node; }
		int numberSource() const { return m_num_source; }
		long patternStep() const { return m_pattern_step; }
		bool indexFlag(size_t i) const { return m_index_flag[i]; }
		long qualityTimeStep() const { return m_quality_time_step; }
		long intervalTimeStep() const { return m_quality_time_step * m_num_quality_records_each_phase; }
		int minStartTime() const { return m_min_start_time; }
		int maxStartTimeSize()const { return (m_max_start_time - m_min_start_time) / m_pattern_step + 1; }
		int minDuration() const { return m_min_duration; }
		int maxDurationSize() const { return (m_max_duration - m_min_duration) / m_pattern_step + 1; }
		int minMultiplier() const { return m_min_multiplier; }
		int maxMultiplier() const { return m_max_multiplier; }
		const std::vector<Real> &getCoordinates(int idx) const { return m_location[idx - 1]; }
		const std::vector<std::vector<Real>> &getCoordinates()const { return m_location; }
		const std::vector<std::vector<bool>> &getAdjmatrix() const { return m_adjmatrix; }
		void setInitType(InitType type) { m_init_type = type; }
		InitType initType() const { return m_init_type; }
		long injectDuration(size_t i) const { return m_inject_duration[i]; }
		const Q_AHC& get_Q_AHC() const { return  m_clusters; }
		Q_AHC& get_Q_AHC() { return  m_clusters; }

		bool isAlgorithmStarted() const { return m_start_algorithm; }
		void setAlgorithmStart() { m_start_algorithm = true; }
		float calculateDistance(int index1, int index2) const;
		void calProByBeVisited();
		const std::vector<std::vector<double>>& getProBeVisited() const { return m_pro_be_visited; }

		void setUseLSTM(bool flag);
	};
}


#endif // !OFEC_CSIWDN_H



