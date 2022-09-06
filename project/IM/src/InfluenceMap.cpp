#include "InfluenceMap.h"


MapPosition::MapPosition(UnitsVec& unit_vector) {
	if (unit_vector.size() != 0) {
		MapPoint pos_center;
		float sum_x = 0, sum_y = 0;
		for (const auto &it : unit_vector) {
			sum_x += it->pos.x;
			sum_y += it->pos.y;
		}
		m_center = MapPoint(sum_x / float(unit_vector.size()), sum_y / float(unit_vector.size()));
		m_top_left = MapPoint(m_center.x - float(MAP_X_LENGTH) / 2.0f, m_center.y + float(MAP_Y_LENGTH) / 2.0f);
		m_top_right = MapPoint(m_center.x + float(MAP_X_LENGTH) / 2.0f, m_center.y + float(MAP_Y_LENGTH) / 2.0f);
		m_bottom_left = MapPoint(m_center.x - float(MAP_X_LENGTH) / 2.0f, m_center.y - float(MAP_Y_LENGTH) / 2.0f);
		m_bottom_right = MapPoint(m_center.x + float(MAP_X_LENGTH) / 2.0f, m_center.y - float(MAP_Y_LENGTH) / 2.0f);
	}
}

InfluenceMap::InfluenceMap() :
	m_map_position(MapPosition()), m_map_alliance(Neutral) {
	m_map_arr;
}

InfluenceMap::InfluenceMap(MapAlliance map_alliance) :
	m_map_position(MapPosition()), m_map_alliance(map_alliance) {
	m_map_arr;
}

InfluenceMap::InfluenceMap(UnitsVec& unit_vector, MapAlliance map_alliance) :
	m_map_position(MapPosition()), m_map_alliance(map_alliance) {
	m_map_arr;
}

//InfluenceMap::InfluenceMap(const InfluenceMap& IM) :
//	m_map_position(IM.getMapPosition()),
//	m_map_alliance(IM.getMapAlliance()) {
//	//std::copy(std::begin(IM.m_map_arr), std::end(IM.m_map_arr), std::begin(m_map_arr));
//}
//
//InfluenceMap& InfluenceMap::operator=(const InfluenceMap& IM) {
//	if (this != &IM) {
//		this->m_map_position = IM.getMapPosition();
//		this->m_map_alliance = IM.getMapAlliance();
//		std::copy(std::begin(IM.m_map_arr), std::end(IM.m_map_arr), std::begin(this->m_map_arr));
//	}
//	return *this;
//}
////
//InfluenceMap & InfluenceMap::operator=(const InfluenceMap&& IM) {
//	if (this != &IM) {
//		this->m_map_position = IM.getMapPosition();
//		this->m_map_alliance = IM.getMapAlliance();
//		//std::copy(std::begin(IM.m_map_arr), std::end(IM.m_map_arr), std::begin(this->m_map_arr));
//	}
//	return *this;
//}

GridPoint InfluenceMap::turnMapToGrid(const MapPoint& map_point) {
	float Xmin = this->m_map_position.getBottomLeft().x;
	float Xmax = this->m_map_position.getTopRight().x;
	float Ymin = this->m_map_position.getBottomRight().y;
	float Ymax = this->m_map_position.getTopLeft().y;

	float gridvalue = float(Xmax - Xmin) / float(MAP_GRID_SIZE);
	GridPoint grid_point(MAP_GRID_SIZE - int((map_point.y - Ymin) / gridvalue), int((map_point.x - Xmin) / gridvalue));
	return GridPoint(grid_point);
}

MapPoint InfluenceMap::turnGridToMap(const GridPoint & grid_point) {
	float Xmin = this->m_map_position.getBottomLeft().x;
	float Xmax = this->m_map_position.getTopRight().x;
	float Ymin = this->m_map_position.getBottomRight().y;
	float Ymax = this->m_map_position.getTopLeft().y;

	float gridvalue = float(Xmax - Xmin) / float(MAP_GRID_SIZE);
	float xmin = Xmin + gridvalue * (grid_point.y);
	float xmax = Xmin + gridvalue * (grid_point.y + 1);
	float ymin = Ymin + gridvalue * (MAP_GRID_SIZE - grid_point.x);
	float ymax = Ymin + gridvalue * (MAP_GRID_SIZE - grid_point.x + 1);

	float ran_x = gridvalue * float(rand()) / RAND_MAX + xmin;
	float ran_y = gridvalue * float(rand()) / RAND_MAX + ymin;
	return MapPoint(ran_x, ran_y);
}

void InfluenceMap::updateIMValue(const UnitsVec& unit_vector) {

	for (auto it = unit_vector.begin(); it != unit_vector.end(); ++it) {
		const sc2::Unit *const unit = *it;
		update_(unit);
	}

}

void InfluenceMap::update_(const sc2::Unit *const unit) {
	MapPoint center_mp = unit->pos;
	GridPoint center_gp = turnMapToGrid(center_mp);
	int max_x = MAP_GRID_SIZE - 1;
	int max_y = MAP_GRID_SIZE - 1;

	for (int i = 0; i < Rule_Size; ++i)
		update_beighbors(center_gp, unit->alliance, i);

}

void InfluenceMap::update_beighbors(GridPoint center, sc2::Unit::Alliance alliance, int level, InfluenceRule rule) {
	int max_x = MAP_GRID_SIZE - 1;
	int max_y = MAP_GRID_SIZE - 1;

	if (alliance == Self) {
		if (level == 0)
			m_map_arr[center.x][center.y] += rule.m_self_level[level];
		else {
			for (int dx = (center.x > 0 ? -1 * abs(level) : 0); dx <= (center.x < max_x ? abs(level) : 0); ++dx) {
				for (int dy = (center.y > 0 ? -1 * abs(level) : 0); dy <= (center.y < max_y ? abs(level) : 0); ++dy) {
					if (dx != 0 || dy != 0) {
						m_map_arr[center.x + dx][center.y + dy] += rule.m_self_level[level];
					}
				}
			}
		}
	}
	else if (alliance == Enemy) {
		if (level == 0)
			m_map_arr[center.x][center.y] += rule.m_enemy_level[level];
		else {
			for (int dx = (center.x > 0 ? -1 * abs(level) : 0); dx <= (center.x < max_x ? abs(level) : 0); ++dx) {
				for (int dy = (center.y > 0 ? -1 * abs(level) : 0); dy <= (center.y < max_y ? abs(level) : 0); ++dy) {
					if (dx != 0 || dy != 0) {
						m_map_arr[center.x + dx][center.y + dy] += rule.m_enemy_level[level];
					}
				}
			}
		}
	}

}

void InfluenceMap::writeIMarrToFile(const std::string foutPath, const std::string fout3rPath) {
	std::ofstream fout1;
	fout1.open(foutPath);
	for (int i = 0; i < MAP_GRID_SIZE; ++i) {
		for (int j = 0; j < MAP_GRID_SIZE; ++j) {
			fout1 << (this->getMapArray())[i][j] << " ";
		}
		fout1 << std::endl;
	}
	fout1.close();

	std::ofstream fout2;
	fout2.open(fout3rPath);
	for (int i = 0; i < MAP_GRID_SIZE; ++i) {
		for (int j = 0; j < MAP_GRID_SIZE; ++j) {
			fout2 << i << " " << j << " " << (this->getMapArray())[MAP_GRID_SIZE - i - 1][j] << std::endl;
		}
		fout2 << std::endl;
	}
	fout2.close();
}


#ifdef _DRAW_
void displayIMarr() {

	Gnuplot gp;
#ifdef _AUTO_CLOSE_PLOT
	if (flagPressQ) {
		pressQ();
	}
#endif
	gp << "set terminal qt title \"title\" position 1930,10" << std::endl;
	gp << "set tic scale 0" << std::endl;
	gp << "set key font \"Times New Roman, 10\" textcolor rgbcolor \"white\"" << std::endl;
	gp << "set cblabel font \"Times New Roman, 8\"" << std::endl;
	gp << "set cbtics scale 0 font \"Times New Roman, 8\"" << std::endl;
	gp << "set xlabel \"x\" offset 0, 0, 0 font \"Times New Roman, 8\"" << std::endl;
	gp << "set ylabel \"y\" norotate offset 1, 0, 0 font \"Times New Roman, 8\"" << std::endl;
	gp << "set xtics font \"Times New Roman, 8\"" << std::endl;
	gp << "set ytics font \"Times New Roman, 8\"" << std::endl;
	gp << "set xrange[-0.5:" << std::to_string(MAP_GRID_SIZE - 0.5) << "] noreverse nowriteback" << std::endl;
	gp << "set yrange[-0.5:" << std::to_string(MAP_GRID_SIZE - 0.5) << "] reverse nowriteback" << std::endl;
	gp << "set view map" << std::endl;
	gp << "set title \"Influence Map with both Selfs and Enemys\" font \"Times New Roman, 12\"" << std::endl;
	gp << "FILE1 = \"D:/bch_sc2_OFEC/sc2api/project/IM/datafile/IM_arr_3row.txt\"" << std::endl;
	gp << "map1 = FILE1" << std::endl;
	gp << "set contour base" << std::endl;

	gp << "plot map1 using 2:1:3 with image t \"Influence Map\", \\" << std::endl;
	gp << "map1 using 2:1:($3 == 0 ?\"\":sprintf(\"%g\",$3)) t \"Influence Map\" with labels font \"Times, 8\" textcolor rgbcolor \"white\"" << std::endl;
	//gp << "plot map1 using 2:1:3 with image t \"Influence Map\"" << std::endl;

#if 0
	//gp << "set view map" << std::endl;
	//gp << "set title \"Influence Map with Selfs\" font \"Times New Roman, 12\"" << std::endl;
	//gp << "FILE2 = \"D:/bch_sc2_OFEC/sc2api/project/IM/datafile/IM_arr_self_3row.txt\"" << std::endl;
	//gp << "map2 = FILE2" << std::endl;
	//gp << "set contour base" << std::endl;
	//gp << "plot map2 using 2:1:3 with image t \"Influence Map\", \\" << std::endl;
	//gp << "map2 using 2:1:($3 == 0 ?\"\":sprintf(\"%g\",$3)) t \"Influence Value\" with labels font \"Times, 8\" textcolor rgbcolor \"white\"" << std::endl;

	//gp << "set view map" << std::endl;
	//gp << "set title \"Influence Map with Enemys\" font \"Times New Roman, 12\"" << std::endl;
	//gp << "FILE3 = \"D:/bch_sc2_OFEC/sc2api/project/IM/datafile/IM_arr_enemy_3row.txt\"" << std::endl;
	//gp << "map3 = FILE3" << std::endl;
	//gp << "set contour base" << std::endl;
	//gp << "plot map3 using 2:1:3 with image t \"Influence Map\", \\" << std::endl;
	//gp << "map3 using 2:1:($3 == 0 ?\"\":sprintf(\"%g\",$3)) t \"Influence Value\" with labels font \"Times, 8\" textcolor rgbcolor \"white\"" << std::endl;

#endif

	++flagPressQ;
}

#endif

#ifdef _WIN32
#ifdef _AUTO_CLOSE_PLOT
void pressQ() {
	keybd_event('Q', 0, 0, 0); //°´ÏÂQ¼ü
	keybd_event('Q', 0, KEYEVENTF_KEYUP, 0);//ËÉ¿ªQ¼ü
}
#endif
#endif

//bool InfluenceMap::turnIMNtoIMS(const UnitsVec& units_enemy) {
//	return true;
//}

MapPoint InfluenceMap::getCenterMapPoint(const std::vector<MapPoint>& vec_mappoint) {
	MapPoint sum = MapPoint();
	int cnt = 0;
	for (int i = 0; i < vec_mappoint.size(); ++i) {
		sum += vec_mappoint.at(i);
	}
	return (sum / vec_mappoint.size());
}

float InfluenceMap::calculateDistance(const MapPoint& point_a, const MapPoint& point_b) {
	return float(sqrt((point_a.x - point_b.x)*(point_a.x - point_b.x) + (point_a.y - point_b.y)*(point_a.y - point_b.y)));
}

float InfluenceMap::calculateWeightDistance(const IMPopVec& popvec, const int index, const MapPoint& point_a, const MapPoint& point_b) {
	float distance1 = sqrt((point_a.x - point_b.x)*(point_a.x - point_b.x) + (point_a.y - point_b.y)*(point_a.y - point_b.y));
	float distance2 = (popvec.at(index).second + 1.0) / 2.0;
	return distance1 * distance2 * distance2;
}

Direction InfluenceMap::getDirection2P(const MapPoint& point_a, const MapPoint& point_b) {
	if (point_a.x <= point_b.x && point_a.y <= point_b.y) {
		return Direction::CSYS1;
	}
	else if (point_a.x >= point_b.x && point_a.y <= point_b.y) {
		return Direction::CSYS2;
	}
	else if (point_a.x >= point_b.x && point_a.y >= point_b.y) {
		return Direction::CSYS3;
	}
	else if (point_a.x <= point_b.x && point_a.y >= point_b.y) {
		return Direction::CSYS4;
	}

}