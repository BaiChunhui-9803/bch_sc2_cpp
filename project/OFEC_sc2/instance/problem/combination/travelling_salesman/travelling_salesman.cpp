#include "travelling_salesman.h"
#include <fstream>
#include <cstring>
#include "../../../../core/instance_manager.h"
#include "../../../../core/global.h"
#include "../../../../core/problem/solution.h"

namespace ofec {
	void TravellingSalesman::initialize_() {
		Problem::initialize_();
		auto &v = GET_PARAM(m_id_param);
		m_file_name = std::get<std::string>(v.at("dataFile1"));
		resizeObjective(1);
		m_optima.clear();
		readProblem();
		m_domain.resize(m_num_cities);
		for (size_t i = 0; i < m_num_cities; ++i)
			m_domain.setRange(0, m_num_cities - 1, i);
	}

	void TravellingSalesman::evaluate_(SolBase& s, bool effective) {
		VarVec<int>& x = dynamic_cast<Solution<VarVec<int>> &>(s).variable();
		std::vector<Real>& obj = dynamic_cast<Solution<VarVec<int>> &>(s).objective();
		std::vector<int> x_(x.begin(), x.end()); //for parallel running
		evaluateObjective(x_.data(), obj);
	}

	void TravellingSalesman::evaluateObjective(int* x, std::vector<Real>& obj) {
		for (size_t i = 0; i < m_num_objs; i++)
			obj[i] = 0;
		int row, col;
		for (size_t n = 0; n < m_num_objs; n++) {
			for (size_t i = 0; i < m_num_cities; i++) {
				row = x[i];
				col = x[(i + 1) % m_num_cities];
				obj[n] += mvvv_cost[n][row][col];
			}
		}
	}

	bool TravellingSalesman::isValid(SolBase& s) const {
		if (!m_if_valid_check)
			return true;

		const VarVec<int>& x = dynamic_cast<const Solution<VarVec<int>> &>(s).variable();

		for (size_t i = 0; i < m_num_cities; i++)  //judge the range		
			if ((x[i]) < m_domain.range(i).limit.first || (x[i]) > m_domain.range(i).limit.second)
				return false;

		std::vector<int> flag(m_num_cities, 0);  //judge whether has the same gene
		int temp;
		for (size_t i = 0; i < m_num_cities; i++)
		{
			temp = x[i];
			flag[temp] = 1;
		}
		for (size_t i = 0; i < m_num_cities; i++)
			if (flag[i] == 0)
				return false;
		return true;
	}

	void TravellingSalesman::initSolution(SolBase& s, int id_rnd) const {
		VarVec<int>& x = dynamic_cast<Solution<VarVec<int>>&>(s).variable();
		for (int idx(0); idx < x.size(); ++idx) {
			x[idx] = idx;
		}
		GET_RND(id_rnd).uniform.shuffle(x.begin(), x.end());
		if (!isValid(s))
			throw MyExcept("error in @TravellingSalesman::initSolution() in travelling_salesman.cpp");
	}

	void TravellingSalesman::initSolutionNN(SolBase& s, int id_rnd) const
	{
		std::vector<int> candidate(m_num_cities);
		int n = 0;
		for (int i(0); i < m_num_cities; ++i) {
			candidate[i] = i;
		}
		//	m_heuristic_sol.variable().resize(m_var_size);
		VarVec<int>& x = dynamic_cast<Solution<VarVec<int>>&>(s).variable();
		x[n] = m_num_cities * GET_RND(id_rnd).uniform.next();
		candidate[x[n]] = candidate[m_num_cities - 1];

		std::cout << x[n] << "\t" << candidate[x[n]] << std::endl;
		++n;
		//candidate[x[n++]] = candidate[m_variable_size - n];
		while (n < m_num_cities) {
			int loc = 0;
			double min = mvvv_cost[0][x[n - 1]][candidate[loc]];
			for (int m = 0; m < m_num_cities - n; m++) {
				if (mvvv_cost[0][x[n - 1]][candidate[m]] < min) {
					min = mvvv_cost[0][x[n - 1]][candidate[m]];
					loc = m;
				}
			}
			x[n++] = candidate[loc];
			candidate[loc] = candidate[m_num_cities - n];
		}

		if (!isValid(s))
			throw MyExcept("error in @travelling_salesman::initialize_solutionNN() in travelling_salesman.cpp");

	}

	bool TravellingSalesman::same(const SolBase& s1, const SolBase& s2) const {
		const VarVec<int>& x1 = dynamic_cast<const Solution<VarVec<int>> &>(s1).variable();
		const VarVec<int>& x2 = dynamic_cast<const Solution<VarVec<int>> &>(s2).variable();
		int i, j, pos = 0;
		for (i = 0; i < m_num_cities; i++)
		{
			if (x1[0] == x2[i])
			{
				pos = i;
				break;
			}
		}
		j = pos;
		for (i = 0; i < m_num_cities; i++)
		{
			if (x1[i] != x2[j])  break;
			else j = (j + 1) % m_num_cities;
		}
		if (i == m_num_cities) return true;
		j = pos;
		for (i = 0; i < m_num_cities; i++)
		{
			if (x1[i] != x2[j])  break;
			else j = (j - 1 + m_num_cities) % m_num_cities;
		}
		if (i == m_num_cities) return true;
		return false;
	}

	Real TravellingSalesman::variableDistance(const SolBase& s1, const SolBase& s2) const {
		static std::vector<std::vector<bool>> mvv_s1_edge(m_num_cities); // a temporary matrix to store the edges of s1, thus helping calculate the distance between two solutions
		const VarVec<int>& x1 = dynamic_cast<const Solution<VarVec<int>> &>(s1).variable();
		const VarVec<int>& x2 = dynamic_cast<const Solution<VarVec<int>> &>(s2).variable();

		for (auto& row : mvv_s1_edge)
			row.assign(m_num_cities, false);
		for (size_t i = 0; i < m_num_cities; i++) {
			mvv_s1_edge[x1[i]][x1[(i + 1) % m_num_cities]] = true;
		}
		Real dis = 0;
		for (size_t i = 0; i < m_num_cities; i++) {
			if (!mvv_s1_edge[x2[i]][x2[(i + 1) % m_num_cities]])dis += 1;
		}
		return dis;
	}

	Real TravellingSalesman::variableDistance(const VarBase& x1, const VarBase& x2) const {
		static std::vector<std::vector<bool>> mvv_s1_edge(m_num_cities); // a temporary matrix to store the edges of s1, thus helping calculate the distance between two solutions
		const VarVec<int>& x1_ = dynamic_cast<const VarVec<int>&>(x1);
		const VarVec<int>& x2_ = dynamic_cast<const VarVec<int>&>(x2);

		for (auto& row : mvv_s1_edge)
			row.assign(m_num_cities, false);
		for (size_t i = 0; i < m_num_cities; i++) {
			mvv_s1_edge[x1_[i]][x1_[(i + 1) % m_num_cities]] = true;
		}
		Real dis = 0;
		for (size_t i = 0; i < m_num_cities; i++) {
			if (!mvv_s1_edge[x2_[i]][x2_[(i + 1) % m_num_cities]])dis += 1;
		}
		return dis;
	}

	std::pair<int, int> TravellingSalesman::adjacentCities(const SolBase& s, int city) const {
		const VarVec<int>& x = dynamic_cast<const Solution<VarVec<int>> &>(s).variable();
		for (size_t i = 0; i < m_num_cities; i++) {
			if (x[i] == city)
				return std::pair<int, int>(x[(i - 1 + m_num_cities) % m_num_cities], x[(i + 1) % m_num_cities]);
		}
		throw MyExcept("error@TravellingSalesman::adjacentCities()");
	}

	void TravellingSalesman::nearbyCities(std::vector<std::vector<int>>& nearby, int num_near, int obj) {
		int i, j, z, pos;
		Real min;
		std::vector<int> visited(m_num_cities);
		int num = num_near == 0 ? nearby[0].size() : num_near;
		for (i = 0; i < m_num_cities; i++)
		{
			for (j = 0; j < m_num_cities; j++)
				visited[j] = 0;
			for (j = 0; j < num - 1; j++)
			{
				min = 0xfffffff;
				for (z = 0; z < m_num_cities; z++)
				{
					if (visited[z] == 0 && i != z && min > mvvv_cost[obj][i][z])
					{
						min = mvvv_cost[obj][i][z];
						pos = z;
					}
				}
				visited[pos] = 1;
				nearby[i][j] = pos;
			}
		}
	}

	void TravellingSalesman::prim(std::vector<std::vector<int>>& mst_edge, int n) {
		std::vector<int> nears(m_num_cities);
		int i, j, k, l;
		int Min = mvvv_cost[n][0][1];
		k = 0; l = 1;
		for (i = 0; i < m_num_cities; i++)  //find the nearest edge in the graph
		{
			for (j = 0; j < m_num_cities; j++)
			{
				if (Min > mvvv_cost[n][i][j] && i != j)
				{
					Min = mvvv_cost[n][i][j];
					k = i; l = j;
				}
			}
		}
		mst_edge[0][0] = k; mst_edge[0][1] = l;
		for (i = 0; i < m_num_cities; i++)  //initial nears
		{
			if (mvvv_cost[n][i][l] < mvvv_cost[n][i][k])
				nears[i] = l;
			else nears[i] = k;
		}
		nears[l] = -1; nears[k] = -1;
		for (i = 2; i < m_num_cities; i++)//find the remain n-2 edges
		{
			Min = 0xfffffff;
			for (int z = 0; z < m_num_cities; z++)
			{
				if (nears[z] != -1 && Min > mvvv_cost[n][z][nears[z]])
				{
					Min = mvvv_cost[n][z][nears[z]];
					j = z;
				}
			}
			mst_edge[i - 1][0] = j; mst_edge[i - 1][1] = nears[j];
			nears[j] = -1;
			for (k = 0; k < m_num_cities; k++)
				if (nears[k] != -1 && mvvv_cost[n][k][nears[k]] > mvvv_cost[n][k][j])
					nears[k] = j;
		}
	}

	void TravellingSalesman::calculateEdgeWeight(char* edge_type) {
		mvvv_cost.resize(m_num_objs);
		for (size_t i = 0; i < m_num_objs; i++)
			mvvv_cost[i].resize(m_num_cities);
		for (size_t i = 0; i < m_num_objs; i++)
			for (size_t j = 0; j < m_num_cities; j++)
				mvvv_cost[i][j].resize(m_num_cities);

		if (!strcmp(edge_type, "EUC_2D")) {
			for (size_t i = 0; i < m_num_cities; i++)
				for (size_t j = 0; j < m_num_cities; j++)
					mvvv_cost[0][i][j] = static_cast<int>(sqrt(pow(m_coordinate[i][0] - m_coordinate[j][0], 2) + pow(m_coordinate[i][1] - m_coordinate[j][1], 2)) + 0.5);
		}
		else if (!strcmp(edge_type, "ATT")) {
			Real r;
			int t;
			for (size_t i = 0; i < m_num_cities; i++)
			{
				for (size_t j = 0; j < m_num_cities; j++)
				{
					r = sqrt((pow(m_coordinate[i][0] - m_coordinate[j][0], 2) + pow(m_coordinate[i][1] - m_coordinate[j][1], 2)) / 10.0);
					t = static_cast<int>(r + 0.5);
					if (t < r) mvvv_cost[0][i][j] = t + 1;
					else mvvv_cost[0][i][j] = t;
				}
			}
		}
		else if (!strcmp(edge_type, "GEO")) {
			Real pi = 3.141592, RRR = 6378.388, min;
			int deg;
			for (size_t i = 0; i < m_num_cities; i++)
			{
				for (size_t j = 0; j < 2; j++)
				{
					deg = static_cast<int>(m_coordinate[i][j]);
					min = m_coordinate[i][j] - deg;
					m_coordinate[i][j] = pi * (deg + 5.0 * min / 3.0) / 180.0;
				}
			}
			Real q1, q2, q3;
			for (size_t i = 0; i < m_num_cities; i++) {
				for (size_t j = 0; j < m_num_cities; j++) {
					q1 = cos(m_coordinate[i][1] - m_coordinate[j][1]);
					q2 = cos(m_coordinate[i][0] - m_coordinate[j][0]);
					q3 = cos(m_coordinate[i][0] + m_coordinate[j][0]);
					mvvv_cost[0][i][j] = static_cast<int>(RRR * acos(0.5 * ((1.0 + q1) * q2 - (1.0 - q1) * q3)) + 1.0);
				}
			}
		}
		else
			throw MyExcept("no exiting this edgeType in function calculateEdgeWeight in travelling_salesman.cpp, please add it here!");
	}

	void TravellingSalesman::readProblem() {

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE__)
#define	strtok_s strtok_r
#endif

		size_t i;
		std::string Line;
		char* edgeType = 0, * edgeFormat = 0;
		char* Keyword = 0;
		char* Type = 0, * Format = 0;
		const char* Delimiters = " ():=\n\t\r\f\v\xef\xbb\xbf";
		std::ostringstream oss1, oss2;
		std::ifstream infile;
		oss1 << static_cast<std::string>(g_working_dir);
		oss1 << "/instance/problem/combination/travelling_salesman/data/" << m_file_name << ".tsp";
		infile.open(oss1.str().c_str());
		if (!infile) {
			throw MyExcept("read travelling salesman data error");
		}
		char* savePtr;
		while (std::getline(infile, Line)) {
			if (!(Keyword = strtok_s((char*)Line.c_str(), Delimiters, &savePtr)))
				continue;
			for (i = 0; i < strlen(Keyword); i++)
				Keyword[i] = toupper(Keyword[i]);
			if (!strcmp(Keyword, "NAME"))
				continue;
			else if (!strcmp(Keyword, "COMMENT"))
				continue;
			else if (!strcmp(Keyword, "TYPE"))
				continue;
			else if (!strcmp(Keyword, "DIMENSION")) {
				char* token = strtok_s(0, Delimiters, &savePtr);
				m_num_cities = atoi(token);
			}
			else if (!strcmp(Keyword, "EDGE_WEIGHT_TYPE")) {
				edgeType = strtok_s(0, Delimiters, &savePtr);
				Type = new char[strlen(edgeType) + 1];
				strncpy(Type, edgeType, strlen(edgeType) + 1);
			}
			else if (!strcmp(Keyword, "EDGE_WEIGHT_FORMAT")) {
				edgeFormat = strtok_s(0, Delimiters, &savePtr);
				Format = new char[strlen(edgeFormat) + 1];
				strncpy(Format, edgeFormat, strlen(edgeFormat) + 1);
			}
			else if (!strcmp(Keyword, "NODE_COORD_SECTION")) {
				i = 0;
				std::vector<Real> temp(2);
				while (infile >> Line)
				{
					infile >> temp[0];
					infile >> temp[1];
					m_coordinate.push_back(temp);
					++i;
					if (i == (size_t)m_num_cities) {
						break;
					}
				}
				break;
			}
			else if (!strcmp(Keyword, "EDGE_WEIGHT_SECTION")) {
				mvvv_cost.resize(1);
				mvvv_cost[0].resize(m_num_cities);
				for (size_t j = 0; j < m_num_cities; j++)
					mvvv_cost[0][j].resize(m_num_cities);
				if (!strcmp(Format, "LOWER_DIAG_ROW")) {
					for (size_t i = 0; i < m_num_cities; i++) {
						for (size_t j = 0; j <= i; j++) {
							infile >> mvvv_cost[0][i][j];
							mvvv_cost[0][j][i] = mvvv_cost[0][i][j];
						}
					}
				}
				else if (!strcmp(Format, "UPPER_DIAG_ROW")) {
					for (size_t i = 0; i < m_num_cities; i++) {
						for (int j = i; j < m_num_cities; j++) {
							infile >> mvvv_cost[0][i][j];
							mvvv_cost[0][j][i] = mvvv_cost[0][i][j];
						}
					}
				}
				else if (!strcmp(Format, "FULL_MATRIX")) {
					for (size_t i = 0; i < m_num_cities; i++)
						for (size_t j = 0; j < m_num_cities; j++)
							infile >> mvvv_cost[0][i][j];
				}
				else
					throw MyExcept("no exiting this edgeFormat in function readProblem in travelling_salesman.cpp, please add it here!");
			}
		}
		infile.close();
		infile.clear();
		if (!Type)
			throw MyExcept("file format error in function readProblem in TravellingSalesman.cpp");
		if (strcmp(Type, "EXPLICIT"))
			calculateEdgeWeight(Type);
		if (Type)
			delete[]Type;
		if (Format)
			delete[]Format;
		oss2 << static_cast<std::string>(g_working_dir);
		oss2 << "/instance/problem/combination/travelling_salesman/data/" << m_file_name << ".opt.tour";
		infile.open(oss2.str().c_str());
		if (!infile)
			throw MyExcept("read travelling salesman optimal tour file error");
		std::string oldLine;
		while (getline(infile, Line)) {
			oldLine = Line;
			if (!(Keyword = strtok_s((char*)Line.c_str(), Delimiters, &savePtr)))
				continue;

			for (i = 0; i < strlen(Keyword); i++)
				Keyword[i] = toupper(Keyword[i]);
			if (!strcmp(Keyword, "TOUR_SECTION")) {
				VarVec<int> temp_opt_var(m_num_cities);
				int temp;
				for (i = 0; i < (size_t)m_num_cities; i++) {
					infile >> temp;
					--temp;
					temp_opt_var[i] = temp;
					//temp_opt_var[i] = TypeVar(temp);
				}
				m_optima.appendVar(temp_opt_var);
				m_optima.setVariableGiven(true);
				std::vector<Real> temp_opt_obj(1);
				evaluateObjective(temp_opt_var.data(), temp_opt_obj);
				m_optima.appendObj(temp_opt_obj);
				m_optima.setObjectiveGiven(true);
				break;
			}
			else if (!strcmp(Keyword, "COMMENT")) { //get optimal cost
				size_t j = 0;
				for (int z = oldLine.size() - 1; z >= 0; z--)
					Line[j++] = oldLine[z];
				Keyword = strtok_s((char*)Line.c_str(), Delimiters, &savePtr);
				size_t len = strlen(Keyword) - 1;
				for (i = 0; i <= len; i++)
					oldLine[i] = Keyword[len - i];
				oldLine[len + 1] = '\0';
				m_optima.appendObj(atof(oldLine.c_str()));
			}
			else
				continue;
		}
		infile.close();
		infile.clear();
	}

	void TravellingSalesman::updateCandidates(const SolBase &sol, std::list<std::unique_ptr<SolBase>> &candidates) const {
		if (candidates.empty())
			candidates.emplace_back(new Solution<VarVec<int>>(dynamic_cast<const Solution<VarVec<int>>&>(sol)));
		else if (sol.dominate(*candidates.front(), m_id_pro))
			candidates.front().reset(new Solution<VarVec<int>>(dynamic_cast<const Solution<VarVec<int>>&>(sol)));
	}

	size_t TravellingSalesman::numOptimaFound(const std::list<std::unique_ptr<SolBase>> &candidates) const {
		if (m_optima.isObjectiveGiven() 
			&& !candidates.empty() 
			&& candidates.front()->objectiveDistance(m_optima.objective(0)) < m_objective_accuracy)
			return 1;
		else
			return 0;
	}

	int TravellingSalesman::updateEvalTag(SolBase &s, int id_alg, bool effective_eval) {
		if (isValid(s)) return EvalTag::kNormalEval;
		else return EvalTag::kInfeasible;
	}

	int selectCityRandom(std::vector<std::vector<int>>& matrix, std::vector<int>& visited, int num, int row, int id_rnd) {
		int i, pos, flag = -1;
		std::vector<int> arr(num);
		for (i = 0; i < num; i++)
			arr[i] = matrix[row][i];
		i = num - 1;
		while (i >= 0) {
			pos = int(i * GET_RND(id_rnd).uniform.next());
			if (visited[arr[pos]] == 1) {
				arr[pos] = arr[i];
				i--;
			}
			else {
				visited[arr[pos]] = 1;
				flag = arr[pos];
				break;
			}
		}
		return flag;
	}

	int selectCityRandom(std::vector<int> visited, int dim, int id_rnd) {
		int i, pos, flag = -1;
		std::vector<int> arr(dim);
		for (i = 0; i < dim; i++)
			arr[i] = i;
		i = dim - 1;
		while (i >= 0) {
			pos = int(i * GET_RND(id_rnd).uniform.next());
			if (visited[arr[pos]] == 1)	{
				arr[pos] = arr[i];
				i--;
			}
			else {
				visited[arr[pos]] = 1;
				flag = arr[pos];
				break;
			}
		}
		return flag;
	}

	int selectCityGreedy(std::vector<std::vector<int>>& matrix, std::vector<int>& visited, int num, int row) {
		int i, flag = -1;
		for (i = 0; i < num; i++) {
			if (visited[matrix[row][i]] == 1)
				continue;
			else {
				visited[matrix[row][i]] = 1;
				flag = matrix[row][i];
				break;
			}
		}
		return flag;
	}
}