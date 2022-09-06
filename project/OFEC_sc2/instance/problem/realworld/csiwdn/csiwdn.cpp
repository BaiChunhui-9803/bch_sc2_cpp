#include "csiwdn.h"
#include "../../../../core/global.h"
#include "../../../../core/instance_manager.h"

using namespace epanet;

namespace ofec {
	std::mutex CSIWDN::ms_mutex_epanet;

	void CSIWDN::initialize_() {
		Problem::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_map_inp.str("");
		m_map_rpt.str("");
		m_hyd.str("");

		m_map_inp << g_working_dir << "/instance/problem/realworld/csiwdn/data/map/" << std::get<std::string>(v.at("dataFile2")) << ".inp";
		m_map_rpt << g_working_dir << "/instance/problem/realworld/csiwdn/data/map/" << std::get<std::string>(v.at("dataFile2")) << ".rpt";
		m_hyd << g_working_dir << "/instance/problem/realworld/csiwdn/data/map/hyd.epa";
		m_file_name.assign(std::get<std::string>(v.at("dataFile1")));
		m_init_type = InitType::Random;
		resizeObjective(1);
		m_opt_mode[0] = OptMode::kMinimize;

		readParameter();
		readConnect();

		long num_WQS = m_total_duration / m_quality_time_step;  // times of water quality simulation (WQS)
		m_num_pattern = m_total_duration / m_pattern_step;
		m_Ciobs.resize(m_num_sensor, std::vector<float>(num_WQS));
		m_optima.appendVar(m_real_PS_read);
		m_optima.appendObj(0);
		m_optima.setVariableGiven(true);
		m_optima.setObjectiveGiven(true);

		m_total_phase = m_total_duration / (m_num_quality_records_each_phase * m_quality_time_step);
		m_phase = m_total_phase;

		m_start_algorithm = false;

		initializeEPANET(); //的到水力信息
		readLocation();  //读取各节点坐标
		getCitByEPANET(m_optima.variable(0), m_Ciobs);
		m_is_detected = false;
		for (int i = 0; i < m_num_sensor; ++i) {
			if (m_is_detected)
				break;
			for (int q = 0; q < num_WQS; ++q) {
				if (m_Ciobs[i][q] > 1e-6) {
					m_first_phase = q / m_num_quality_records_each_phase + 1;
					m_is_detected = true;
					break;
				}
			}
		}

		m_index_flag.resize(m_num_node, true);
		updateAdjmatrix();
		m_clusters.clear();

		m_num_be_visited.resize(m_num_source, std::vector<int>(m_num_node));

		m_phase = m_first_phase;  //1
		if (m_phase < 0) throw MyExcept("No detection!");

		m_index_flag.resize(m_num_node, true);

		m_objective_accuracy = 1.e-6;

		setUseLSTM(true);
	}

	float CSIWDN::calculateDistance(int index1, int index2) const {
		return sqrt(pow(m_location[index1 - 1][0] - m_location[index2 - 1][0], 2) + pow(m_location[index1 - 1][1] - m_location[index2 - 1][1], 2));
	}

	void CSIWDN::readParameter() {
		size_t i;
		std::string Line;
		char label[32];
		char *Keyword = 0;
		const char *Delimiters = " ():=\n\t\r\f\v\xef\xbb\xbf";  //分隔符
		std::ostringstream oss1;
		std::ifstream infile;
		oss1 << g_working_dir << "/instance/problem/realworld/csiwdn/data/input/" << m_file_name << ".txt";
		infile.open(oss1.str().c_str()); //c_str()将string类型转换为char*类型
		if (!infile) {
			throw MyExcept("read epanet data error");
		}
		char *savePtr;
		while (getline(infile, Line))
		{
			if (!(Keyword = gStrtok_r((char *)Line.c_str(), Delimiters, &savePtr)))//？？？
				continue;
			for (i = 0; i < strlen(Keyword); ++i)
				Keyword[i] = toupper(Keyword[i]); //小写字母转为大写

			if (!strcmp(Keyword, "NUMSENSOR")) {
				char *token = gStrtok_r(0, Delimiters, &savePtr);
				m_num_sensor = atoi(token);
				m_sensor_Loc.resize(m_num_sensor);
			}
			else if (!strcmp(Keyword, "NUMSOURCE")) {
				char *token = gStrtok_r(0, Delimiters, &savePtr);
				m_num_source = atoi(token);
				m_real_PS_read.resize(m_num_source);
			}
			else if (!strcmp(Keyword, "SET_DURATIONRANGE")) {
				infile >> m_min_duration;
				infile >> m_max_duration;
			}
			else if (!strcmp(Keyword, "SET_STARTTIMERANGE")) {
				infile >> m_min_start_time;
				infile >> m_max_start_time;
			}
			else if (!strcmp(Keyword, "SET_MULTIPLIERRANGE")) {
				infile >> m_min_multiplier;
				infile >> m_max_multiplier;
			}
			else if (!strcmp(Keyword, "SET_SENSORLOC"))
			{
				for (i = 0; i < m_num_sensor; i++)
					infile >> m_sensor_Loc[i];
			}
			else if (!strcmp(Keyword, "SET_TOTALDURATION"))
			{
				infile >> m_total_duration;
			}
			else if (!strcmp(Keyword, "SET_QUALITYTIMESTEP"))
			{
				infile >> m_quality_time_step;
			}
			else if (!strcmp(Keyword, "SET_PATTERNSTEP"))
			{
				infile >> m_pattern_step;
			}
			else if (!strcmp(Keyword, "SET_TIMEINTERVAL"))
			{
				infile >> m_num_quality_records_each_phase;
			}
			else if (!strcmp(Keyword, "LOC"))
			{
				for (i = 0; i < m_num_source; ++i) {
					//infile >> TempChar;
					//getline(infile,label,' ');
					//m_globalOpt[i].data().m_x[0].Loc = Temp;
					//infile.get(label,32,' ');
					//strcpy(m_real_PS_read[i].location(), label);
					infile >> m_real_PS_read.location(i);
				}
			}
			else if (!strcmp(Keyword, "DURATION"))
			{
				m_inject_duration.resize(m_num_source);
				for (i = 0; i < m_num_source; ++i) {
					infile >> m_real_PS_read.duration(i);
					m_inject_duration[i] = m_real_PS_read.duration(i);
				}
			}
			else if (!strcmp(Keyword, "SOURCE"))
			{
				for (i = 0; i < m_num_source; ++i) {
					infile >> m_real_PS_read.source(i);
				}
			}
			else if (!strcmp(Keyword, "STARTTIME"))
			{
				for (i = 0; i < m_num_source; ++i) {
					infile >> m_real_PS_read.startTime(i);
				}
			}
			else if (!strcmp(Keyword, "MULTIPLIER"))
			{
				size_t mul_size;
				for (i = 0; i < m_num_source; ++i) {
					mul_size = (size_t)(m_real_PS_read.duration(i) / m_pattern_step);
					m_real_PS_read.multiplier(i).resize(mul_size, 0);
					for (size_t j = 0; j < mul_size; ++j) {
						infile >> m_real_PS_read.multiplier(i)[j];
					}
				}
			}
		}
		infile.close();
		infile.clear();
	}

	void CSIWDN::evaluate_(SolBase &s, bool effective) {
		VarCSIWDN &x = dynamic_cast<Solution<VarCSIWDN> &>(s).variable();
		auto &obj = dynamic_cast<Solution<VarCSIWDN> &>(s).objective();

		evaluateObjective(x, obj);
		int z = endSourceIdx(), q = startSourceIdx();
		for (int j = q; j <= z; ++j) {
			++m_num_be_visited[j][x.index(j) - 1];
		}
	}

	void CSIWDN::evaluateObjective(VarCSIWDN &x, std::vector<Real> &obj) {    //  single evaluation
		int num = m_phase * m_num_quality_records_each_phase;
		std::vector<std::vector<float>> Cit(m_num_sensor, std::vector<float>(num));
		float temp = 0;
		if (m_use_lstm_model)
			getCitByLSTM(x, Cit);
		else
			getCitByEPANET(x, Cit);
		
		for (size_t i = 0; i < num; ++i) {
			for (size_t j = 0; j < m_num_sensor; ++j) {
				temp += pow(m_Ciobs[j][i] - Cit[j][i], 2);
			}
		}
		obj[0] = sqrt(temp / (m_num_sensor * num));
	}

	CSIWDN::~CSIWDN() {
		ENclose();
	}

	void CSIWDN::getCitByEPANET(VarCSIWDN &sol, std::vector<std::vector<float>> &data) {
		ENusehydfile(const_cast<char*> (m_hyd.str().data()));
		ENopenQ();
		std::vector<float> pattern_value;
		for (int i = 0; i < m_num_source; i++) {
			pattern_value.assign(m_num_pattern, 0.0);
			for (int j = 0; j < sol.multiplier(i).size(); ++j) {
				int period = sol.startTime(i) / m_pattern_step + j; //开始后的第几个阶段，10分钟一个阶段
				pattern_value[period] = sol.multiplier(i)[j];  //该阶段的乘子
			}
			ENsetpattern(m_pattern_index[i], pattern_value.data(), m_num_pattern);  //为指定的时间模式设置所有的乘数因子
			if (sol.flagLocation(i)) //是否是真实源头的标示
				ENgetnodeindex(sol.location(i), &sol.index(i));
			else
				ENgetnodeid(sol.index(i), sol.location(i));
			ENsetnodevalue(sol.index(i), EN_SOURCEQUAL, sol.source(i));//设置源头水质
			ENsetnodevalue(sol.index(i), EN_SOURCEPAT, (float)m_pattern_index[i]);//时间模式的索引
			ENsetnodevalue(sol.index(i), EN_SOURCETYPE, EN_FLOWPACED);
		}
		ENinitQ(0);
		long count = 0;
		int z = 0;
		long tstep;
		long t;
		int index;
		char label[32];
		float c;
		do {
			ENrunQ(&t);
			ENstepQ(&tstep);
			if ((z + 1) < m_num_source && t == sol.startTime(z + 1))
				z++;
			for (int i = 0; i < m_num_sensor; ++i) {
				size_t j;
				for (j = 0; j < m_sensor_Loc[i].size(); ++j)
					label[j] = m_sensor_Loc[i][j];
				label[j] = '\0';
				ENgetnodeindex(label, &index);
				ENgetnodevalue(index, EN_QUALITY, &c);
				data[i][t / m_quality_time_step] = c;
			}
//#ifdef OFEC_DEMO
//			m_node_quality.resize(m_num_node, std::vector<float>(m_total_duration / m_quality_time_step));
//			for (int i = 0; i < m_num_node; ++i) {
//				ENgetnodeindex((char *)m_node_name[i].c_str(), &index);
//				ENgetnodevalue(index, EN_QUALITY, &c);
//				m_node_quality[i][t / m_quality_time_step] = c;
//			}
//#endif
			++count;
			if (count == m_num_quality_records_each_phase * m_phase) break;  //分时段
		} while (tstep > 0); //tstep=0表示模拟结束
		ENcloseQ();
	}

	torch::Device g_device(torch::cuda::is_available() ? "cuda" : "cpu");

	void CSIWDN::initializeLSTM() {
		m_input_size = 2;
		m_output_size = m_num_sensor;
		m_hidden_size = 30;
		m_num_layers = 4;
		m_lstm_model = std::make_unique<LstmLinear>(m_input_size, m_hidden_size, m_output_size, m_num_layers);
		m_input_range.resize(m_input_size);
		m_input_range[0] = { 0, m_max_multiplier };
		m_input_range[1] = { m_min_duration, m_max_duration };
		m_output_range.resize(m_output_size);
		for (size_t i = 0; i < m_output_size; i++)
			m_output_range[i] = { m_min_multiplier, m_max_multiplier };
		m_model_file_name = g_working_dir + "/instance/problem/realworld/csiwdn/data/lstm_model/" + m_file_name +".pt";
		torch::load(*m_lstm_model, m_model_file_name);
		//(*m_lstm_model)->to(g_device);
		m_lstm_model_initialized = true;
	}

	void CSIWDN::getCitByLSTM(VarCSIWDN &sol, std::vector<std::vector<float>> &data) {
		if (!m_lstm_model_initialized)
			initializeLSTM();
		int num_quality_records = m_num_quality_records_each_phase * m_phase;
		std::vector<std::vector<float>> inputs(m_num_source, std::vector<float>(num_quality_records * m_input_size));
		std::vector<torch::Tensor> t_inputs(m_num_source), t_outputs(m_num_source);
		torch::Tensor sum_output;

		for (size_t k = 0; k < m_num_source; k++) {
			for (size_t i = 0; i < num_quality_records; i++) {
				inputs[k][i * m_input_size] = 0;
				inputs[k][i * m_input_size + 1] = (i * m_quality_time_step - m_input_range[1].first)
					/(m_input_range[1].second- m_input_range[1].first);
			}
		}

		for (size_t k = 0; k < m_num_source; k++) {
			for (int time = sol.startTime(k); time < sol.startTime(k) + sol.duration(k); time += m_quality_time_step) {
				if (time / (m_num_quality_records_each_phase * m_quality_time_step) > m_phase - 1)
					break;
				float value = (sol.multiplier(k)[(time - sol.startTime(k)) / m_pattern_step] - m_input_range[0].first)
					/ (m_input_range[0].second - m_input_range[0].first);
				inputs[k][time / m_quality_time_step * m_input_size] = value;
			}
			t_inputs[k] = torch::from_blob(inputs[k].data(), { 1, num_quality_records, m_input_size });
			//t_inputs[k] = torch::from_blob(inputs[k].data(), { 1, num_quality_records, m_input_size }).to(g_device);
			t_outputs[k] = (*m_lstm_model)(t_inputs[k]);
			std::cout << t_inputs[k] << std::endl;
			std::cout << t_outputs[k] << std::endl;
			if (k == 0)
				sum_output = t_outputs[k];
			else
				sum_output += t_outputs[k];
		}

		for (size_t i = 0; i < m_output_size; i++) {
			for (size_t j = 0; j < num_quality_records; j++) {
				data[i][j] = sum_output[0][j][i].item().toFloat()
					* (m_output_range[i].second - m_output_range[i].first)
					+ m_output_range[i].first;
			}
		}
	}

	void CSIWDN::readLocation() {
		bool flag = false;
		m_location.resize(m_num_node, std::vector<Real>(2));
		m_node_name.resize(m_num_node);
		std::string line;
		std::ifstream ifname;
		ifname.open(m_map_inp.str());
		size_t i = 0;
		while (getline(ifname, line)) {
			//line.pop_back();  //  (only added in Liunx) if run in windows, please remove this sentence.
			if (line == "[COORDINATES]") {
				flag = true;
				continue;
			}
			if (i == m_num_node)
				break;
			if (flag) {
				ifname >> m_node_name[i];
				m_node_id[m_node_name[i]] = i;
				ifname >> m_location[i][0];
				ifname >> m_location[i][1];
				++i;
			}
		}
		ifname.close();
	}

	void CSIWDN::initializeEPANET() {
		ms_mutex_epanet.lock();
		ENopen(const_cast<char *>(m_map_inp.str().data()), const_cast<char *>(m_map_rpt.str().data()), "");   //  open input and report file 
		ms_mutex_epanet.unlock();

		ENsettimeparam(EN_DURATION, m_total_duration);     //  set Total Duration
		ENsettimeparam(EN_HYDSTEP, m_quality_time_step);         //  set Hydraulic Time Step
		ENsettimeparam(EN_REPORTSTEP, 60 * 60);      //  set Report Time Step
		ENsettimeparam(EN_REPORTSTART, 0);         //  set Report Start Time
		ENgetcount(EN_NODECOUNT, &m_num_node);      // get number of node
		ENsettimeparam(EN_QUALSTEP, m_quality_time_step);        //  set Quality Time Step 10mins
		ENsettimeparam(EN_PATTERNSTEP, m_pattern_step);     //  set Pattern Time Step
		ENsettimeparam(EN_PATTERNSTART, 0);        //  set Pattern Start Time
		ENsetqualtype(EN_CHEM, "Chlorine", "mg/L", ""); // set type of source

		m_pattern_index.resize(m_num_source);
		for (size_t i = 0; i < m_num_source; i++) {
			std::string pattern_id = "contaminant" + std::to_string(i + 1);
			ENaddpattern(pattern_id.data());
			ENgetpatternindex(pattern_id.data(), &m_pattern_index[i]);
		}

		long tstep, t = 0;
		long count = 0;
		int index;
		ENopenH();
		ENinitH(01);
#ifdef OFEC_DEMO
		long num_WHS = m_total_duration / m_quality_time_step;
		for (int i = 0; i < m_pipe.size(); i++)
			m_pipe[i].flow_rate.resize(num_WHS);
		float c;
#endif
		while (true) {
			ENrunH(&t);
			ENnextH(&tstep);
			if (tstep == 0)
				break;
#ifdef OFEC_DEMO
			for (int i = 0; i < m_pipe.size(); i++) {
				ENgetlinkindex((char *)m_pipe[i].pipe_id.c_str(), &index);
				ENgetlinkvalue(index, EN_FLOW, &c);
				m_pipe[i].flow_rate[t / m_quality_time_step] = c;
			}
#endif
			++count;
		}
		ENsavehydfile(const_cast<char *> (m_hyd.str().data()));
		ENcloseH();
	}

	void CSIWDN::updateAdjmatrix() {
		m_adjmatrix.resize(m_node_id.size());
		for (size_t i = 0; i < m_node_id.size(); i++) {
			m_adjmatrix[i].resize(m_node_id.size());
			for (size_t j = 0; j < m_adjmatrix[i].size(); j++)
				m_adjmatrix[i][j] = false;
		}
		for (size_t i = 0; i < m_pipe.size(); i++) {
			int m = m_node_id[m_pipe[i].sNode];
			int n = m_node_id[m_pipe[i].eNode];
			m_adjmatrix[m][n] = true;
			m_adjmatrix[n][m] = true;
		}
	}

	void CSIWDN::readConnect() {
		bool flag = false;
		bool pump = false;
		bool valve = false;

		std::string line;
		std::ifstream ifname;
		//std::pair<std::string, double> pipe;
		ifname.open(m_map_inp.str());
		//size_t i = 0;
		std::string pipe_id;
		//std::string pipe_length;
		//std::vector<std::string> connect(2);
		Pipe p;
		//double diameter;
		while (getline(ifname, line)) {
			if (line == "[PIPES]") {
				flag = true;
				continue;
			}
			if (flag) {
				ifname >> pipe_id;
				if (pipe_id == "[PUMPS]") {
					getline(ifname, line);
					pump = true;
					continue;
				}
				else if (pipe_id == "[VALVES]") {
					getline(ifname, line);
					valve = true;
					continue;
				}
				else if (pipe_id == "[TAGS]")
					break;
				ifname >> p.sNode;
				ifname >> p.eNode;
				//m_connect.push_back(connect);
				if (!pump && !valve) {
					ifname >> p.length;
					ifname >> p.diameter;
					m_pipe.push_back(p);
					//m_pipe.push_back(std::make_pair(pipe_id, diameter));
				}
				else if (pump) {
					p.length = 10;
					p.diameter = 0;
					m_pipe.push_back(p);
					//m_pipe.push_back(std::make_pair(pipe_id, 0));
				}
				else if (valve) {
					ifname >> p.diameter;
					p.length = 10;
					m_pipe.push_back(p);
					//m_pipe.push_back(std::make_pair(pipe_id, diameter));
				}
			}
		}
		ifname.close();
	}

	void CSIWDN::phaseNext() {
		++m_phase;
		m_idx_source = 0;
		while ((m_idx_source + 1) < m_num_source && m_phase >= m_real_PS_read.startTime(m_idx_source + 1) / m_num_quality_records_each_phase) {
			m_idx_source++;
		}
	}

	void CSIWDN::calProByBeVisited() {
		int z = 0, q = 0;
		while ((z + 1) < m_num_source && m_phase >= (m_optima.variable(0).startTime(z + 1) / (m_num_quality_records_each_phase * m_quality_time_step))) {
			z++;
		}
		//int n = m_phase / m_eval_phase;
		//while ((q + 1) < m_num_source && (n * evalPhase()) >= ((m_optima.variable(0).startTime(q + 1) / intervalTimeStep()))) {
		//	q++;
		//}
		m_pro_be_visited.clear();
		m_pro_be_visited.resize(m_num_be_visited.size());
		for (int j = q; j <= z; ++j) {
			int max = 0;
			for (auto &i : m_num_be_visited[j])
				if (max < i) max = i;
			std::vector<int> inverse;
			for (auto &i : m_num_be_visited[j])
				inverse.push_back(max - i);
			int sum = 0;
			for (auto &i : inverse)
				sum += i;
			for (auto &i : inverse)
				m_pro_be_visited[j].push_back((double)i / (double)sum);
		}
	}

	void CSIWDN::setUseLSTM(bool flag) {
		m_use_lstm_model = flag;
		if (m_use_lstm_model)
			initializeLSTM();
		else {
			m_lstm_model.reset();
			m_lstm_model_initialized = false;
		}
	}

	bool CSIWDN::isTimeOut() const {
		return (m_total_duration / (m_num_quality_records_each_phase * m_quality_time_step) < m_phase);
	}

	int CSIWDN::startSourceIdx() const {
		int q = 0;
		//int n = m_phase / m_eval_phase;
		//while ((q + 1) < m_num_source && (n * evalPhase()) >= ((m_optima.variable(0).startTime(q + 1) / intervalTimeStep()))) {
		//	q++;
		//}
		return q;
	}

	int CSIWDN::endSourceIdx() const {
		int z = 0;
		while ((z + 1) < numberSource() && phase() >= ((m_real_PS_read.startTime(z + 1) / intervalTimeStep()))) {
			z++;
		}
		return z;
	}

	void CSIWDN::initSolution(SolBase &s, int id_rnd) const {
		VarCSIWDN &var = dynamic_cast<Solution<VarCSIWDN> &>(s).variable();
		for (size_t z = 0; z < m_num_source; z++) {
			var.flagLocation(z) = false;

			if (m_init_type == InitType::AHC) {
				int size_clusters = m_clusters.cluster_size();
				int random_identifier;
				int size;
				do {
					random_identifier = GET_RND(id_rnd).uniform.nextNonStd<int>(0, size_clusters);
					size = m_clusters.clusters()[random_identifier].member.size();
				} while (size <= 0);
				int random = GET_RND(id_rnd).uniform.nextNonStd<int>(0, size);
				var.index(z) = m_clusters.clusters()[random_identifier].member[random] + 1;
			}
			else if (m_init_type == InitType::Random) {
				var.index(z) = GET_RND(id_rnd).uniform.nextNonStd<int>(1, m_num_node + 1);
			}
			else if (m_init_type == InitType::Distance) {
				std::vector<Real> roulette(m_num_node + 1, 0);
				for (size_t i = 0; i < roulette.size() - 1; ++i) {
					roulette[i + 1] = roulette[i] + m_distance_node[i];
				}
				Real random_value = GET_RND(id_rnd).uniform.nextNonStd<Real>(roulette[0], roulette[roulette.size() - 1]);
				for (size_t i = 0; i < roulette.size() - 1; ++i) {
					if (random_value >= roulette[i] && random_value < roulette[i + 1]) {
						var.index(z) = i + 1;
					}
				}
			}
			else throw MyExcept("No this initialization type");

			var.source(z) = 1.0;
			var.startTime(z) = const_cast<VarCSIWDN &>(m_optima.variable(0)).startTime(z);

			var.duration(z) = const_cast<VarCSIWDN &>(m_optima.variable(0)).duration(z);
			size_t size = var.duration(z) / m_pattern_step;
			if (var.duration(z) % m_pattern_step != 0) ++size;
			var.multiplier(z).resize(size);
			for (auto &j : var.multiplier(z)) {
				j = GET_RND(id_rnd).uniform.nextNonStd<float>(m_min_multiplier, m_max_multiplier);
			}

		}
	}
}
