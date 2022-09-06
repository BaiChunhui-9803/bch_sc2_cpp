#include "GA_micro.h"

void GA_micro::map_insert_(MAP_MYSOL& sol_map, const ValueMySol& sol_value) {
	sol_map.insert(MAP_MYSOL::value_type(sol_value.v_gameloop, std::pair<sc2::ABILITY_ID, sc2::Point3D>(sol_value.v_ability_id, sol_value.v_point)));
}

void GA_micro::initialize_Algorithm(const GameInfomation& game_info) {

	//根据经验信息生成9x9栅格地图并输出至文件
	grid_map();
	unsigned seed;
	seed = time(0);
	srand(seed);

 //   ValueMySol sol_value(100, sc2::ABILITY_ID::INVALID, sc2::Point3D(0, 0, 0));
 //   ValueMySol sol_value2(50, sc2::ABILITY_ID::INVALID, sc2::Point3D(0, 0, 0));
 //   map_insert_(sol_map, sol_value);
 //   map_insert_(sol_map, sol_value2);

	MAP_MYSOL sol_map;
	sc2::Point3D begin_map(55.0, 65.0, 8.0);
	GridPoint begin_grid = map_to_grid(begin_map);
	//std::cout << begin_grid.XPoint << " " << begin_grid.YPoint << std::endl;
	sc2::Point3D next_map = begin_map;

	ValueMySol sol_value_begin(0, sc2::ABILITY_ID::SMART, begin_map);
	map_insert_(sol_map, sol_value_begin);

	for (int i = 1; i < SolutionLength; i++) {
		GridPoint next_grid = select_grid(map_to_grid(next_map));
		next_map = grid_to_map(next_grid);
		ValueMySol sol_value_next(i, sc2::ABILITY_ID::SMART, next_map);
		map_insert_(sol_map, sol_value_next);
		//std::cout << next_grid.XPoint << " " << next_grid.YPoint << std::endl;
	}

	m_sol_vec.push_back(sol_map);

	//for (int i = 0; i < 10; i++) {
	//	double ran_x = 50.0 * rand() / double(RAND_MAX) + 20.0;
	//	double ran_y = 50.0 * rand() / double(RAND_MAX) + 40.0;
	//	ValueMySol sol_value_random((i + 1), sc2::ABILITY_ID::SMART, sc2::Point3D(ran_x, ran_y, game_info.vec_unit.at(0).n_pos.z));
	//	map_insert_(sol_map, sol_value_random);
	//}
}

MAP_MYSOL GA_micro::get_random_solution() {
	return MAP_MYSOL(m_sol_vec.at(0));
}

std::vector<MAP_MYSOL> GA_micro::get_random_solution_vec() {
	return std::vector<MAP_MYSOL>(m_sol_vec);
}

std::vector<MAP_MYSOL> GA_micro::Algoritrm() {

	//while (1) {
	//	int x, y;
	//	std::cin >> x >> y;
	//	GridPoint point_grid(x, y);
	//	sc2::Point3D point_map = grid_to_map(point_grid);
	//	GridPoint point_grid_2 = map_to_grid(point_map);
	//	int a = 1;
	//}



	return std::vector<MAP_MYSOL>();
}

void GA_micro::grid_map() {

	std::ifstream fin;
	fin.open("D:/bch_sc2_OFEC/sc2api/project/micro_kiting/datafile/kiting_pre_map_pre.txt",std::ios::in);
	int N = 100;
	int Xmin = 25, Xmax = 60, Ymin = 50, Ymax = 85;

	double gridvalue = double(Xmax - Xmin) / double(GridSize);
	MapPoint GridMap_Ori[MapSize][MoveSize];

	std::string line;
	std::string str_number = "";
	int i = 0;
	while (std::getline(fin, line)) {
		int j = 0;
		for (auto c : line) {
			if (c != '\t') {
				str_number.push_back(c);
			}
			else {
				if (!(j % 2)) {
					GridMap_Ori[i][j / 2].XPoint = std::stod(str_number);
				}
				else {
					GridMap_Ori[i][j / 2].YPoint = std::stod(str_number);
				}
				str_number.clear();
				j++;
			}
		}
		i++;
	}

	for (int i = 0; i < MapSize; i++) {
		for (int j = 0; j < MoveSize; j++) {
			if (GridMap_Ori[i][j].XPoint >= Xmin && GridMap_Ori[i][j].XPoint <= Xmax && GridMap_Ori[i][j].YPoint >= Ymin && GridMap_Ori[i][j].YPoint <= Ymax) {
			int XCoor = (GridMap_Ori[i][j].XPoint - Xmin) / gridvalue;
			int YCoor = (GridMap_Ori[i][j].YPoint - Ymin) / gridvalue;
			GridMap[GridSize - YCoor][XCoor]++;
			}
		}
	}

	std::ofstream fout;
	fout.open("D:/bch_sc2_OFEC/sc2api/project/micro_kiting/datafile/kiting_gridmap.txt");
	for (int i = 0; i < GridSize; i++) {
		for (int j = 0; j < GridSize; j++) {
			fout << GridMap[i][j] << '\t';
		}
		fout << std::endl;
	}
	fout.close();

	//计算栅格地图的概率和选择概率
	double sum = 0.0;
	for (int i = 0; i < GridSize; i++) {
		for (int j = 0; j < GridSize; j++) {
			sum += GridMap[i][j];
		}
	}

	double last_p = 0.0;
	for (int i = 0; i < GridSize; i++) {
		for (int j = 0; j < GridSize; j++) {
			GridMap_P[i][j] = GridMap[i][j] / sum;
			GridMap_SP[i][j] = GridMap_P[i][j] + last_p;
			last_p = GridMap_SP[i][j];
		}
	}

	//for (int i = 0; i < GridSize; i++) {
	//	for (int j = 0; j < GridSize; j++) {
	//		std::cout << GridMap_P[i][j] << "**" << GridMap_SP[i][j] << '\t';
	//	}
	//	std::cout << std::endl;
	//}
}

GridPoint GA_micro::map_to_grid(sc2::Point3D map_point) {
	int Xmin = 25, Xmax = 60, Ymin = 50, Ymax = 85;
	double gridvalue = double(Xmax - Xmin) / double(GridSize);
	GridPoint grid_point(GridSize - int((map_point.y - Ymin) / gridvalue), (map_point.x - Xmin) / gridvalue);
	return GridPoint(grid_point);
}

sc2::Point3D GA_micro::grid_to_map(GridPoint grid_point) {
	int Xmin = 25, Xmax = 60, Ymin = 50, Ymax = 85;
	double gridvalue = double(Xmax - Xmin) / double(GridSize);
	double xmin = Xmin + gridvalue * (grid_point.YPoint);
	double xmax = Xmin + gridvalue * (grid_point.YPoint + 1);
	double ymin = Ymin + gridvalue * (GridSize - grid_point.XPoint);
	double ymax = Ymin + gridvalue * (GridSize - grid_point.XPoint + 1);

	double ran_x = gridvalue * double(rand()) / RAND_MAX + xmin;
	double ran_y = gridvalue * double(rand()) / RAND_MAX + ymin;
	sc2::Point3D point(ran_x, ran_y, 8.0);

	return sc2::Point3D(point);
}

GridPoint GA_micro::select_grid(GridPoint grid_point) {
	std::vector<GridPoint> beighbors_vec = find_beighbors(grid_point);
	std::vector<GridPoint> select_vec;
	for (auto i : beighbors_vec) {
		if (GridMap[i.XPoint][i.YPoint] < GridMap[grid_point.XPoint][grid_point.YPoint] && GridMap[i.XPoint][i.YPoint] != 0) {
			//std::cout << i.XPoint << "," << i.YPoint << " ";
			select_vec.push_back(i);
		}
	}
	if (select_vec.empty()) {
		select_vec = beighbors_vec;
	}
	//std::cout << std::endl;
	GridPoint point = Lottery_from_vector(select_vec);
	return GridPoint(point);
}

std::vector<GridPoint> GA_micro::find_beighbors(GridPoint grid_point) {
	int max_x = GridSize - 1;
	int max_y = GridSize - 1;
	std::vector<GridPoint> list;

	for (int dx = (grid_point.XPoint > 0 ? -1 : 0); dx <= (grid_point.XPoint < max_x ? 1 : 0); ++dx) {
		for (int dy = (grid_point.YPoint > 0 ? -1 : 0); dy <= (grid_point.YPoint < max_y ? 1 : 0); ++dy) {
			if ((dx == 0 || dy == 0) && (dx + dy != 0)) {
				GridPoint point(grid_point.XPoint + dx, grid_point.YPoint + dy);
				list.push_back(point);
			}
		}
	}

	return std::vector<GridPoint>(list);
}

GridPoint GA_micro::Lottery_from_vector(const std::vector<GridPoint>& vec) {

	int point_x, point_y;
	double sum = 0.0;
	for (auto i : vec) {
		sum += GridMap[i.XPoint][i.YPoint];
	}

	std::vector<double> select_p_vec;
	std::vector<double> select_sp_vec;
	double last_p = 0.0;
	for (int i = 0; i < vec.size(); i++) {
		select_p_vec.push_back(GridMap[vec.at(i).XPoint][vec.at(i).YPoint] / sum);
		select_sp_vec.push_back(GridMap[vec.at(i).XPoint][vec.at(i).YPoint] / sum + last_p);
		last_p = select_sp_vec.at(i);
	}

	std::random_device e;
	std::uniform_real_distribution<double> u(0, 1);
	double ran = u(e);

	for (int i = 0; i < vec.size(); i++) {
		if (ran <= select_sp_vec.at(i)) {
			point_x = vec.at(i).XPoint;
			point_y = vec.at(i).YPoint;
			break;
		}
	}

	GridPoint select_point(point_x, point_y);
	return GridPoint(select_point);
}