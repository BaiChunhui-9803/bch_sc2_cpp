#include "MyAlgorithm.h"



void MyAlgorithm::initializeAlgorithm(const GameInfomation& game_info, const IMPopVec& IM_pop) {
	myal_vec_sol.clear();
	myal_game_info = game_info;
	myal_IM_pop = IM_pop;
	std::vector<MapPoint> vec_mappoint;
	for (auto it = game_info.info_vec_unit.begin(); it != game_info.info_vec_unit.end(); ++it) {
		if (it->n_alliance == Self) {
			MapPoint point(it->n_pos.x, it->n_pos.y);
			vec_mappoint.push_back(point);
		}
	}
	myal_center_self = myal_IM.getCenterMapPoint(vec_mappoint);

}

bool MyAlgorithm::updateAlgorithm(const GameInfomation& game_info, const IMPopVec& IM_pop) {
	myal_game_info = game_info;
	myal_IM_pop = IM_pop;
	std::vector<MapPoint> vec_self, vec_enemy;
	for (auto it = game_info.info_vec_unit.begin(); it != game_info.info_vec_unit.end(); ++it) {
		if (it->n_alliance == Self) {
			MapPoint point(it->n_pos.x, it->n_pos.y);
			vec_self.push_back(point);
		}
		else if (it->n_alliance == Enemy) {
			MapPoint point(it->n_pos.x, it->n_pos.y);
			vec_enemy.push_back(point);
		}
	}
	myal_center_self = myal_IM.getCenterMapPoint(vec_self);
	myal_center_enemy = myal_IM.getCenterMapPoint(vec_enemy);
	return 1;
}

MapPoint MyAlgorithm::getCenterSelf() {
	return myal_center_self;
}

MapPoint MyAlgorithm::getCenterEnemy() {
	return MapPoint();
}

sc2::Tag MyAlgorithm::findNearsetPoint() {
	float distance;
	float distance_min = 99.0f;
	int index = 0;
	for (int i = 0; i < myal_game_info.info_vec_unit.size(); ++i) {
		if (myal_game_info.info_vec_unit.at(i).n_alliance == Enemy) {
			float distance = myal_IM.calculateWeightDistance(
				myal_IM_pop,
				myal_game_info.info_vec_unit.at(i).n_pop,
				myal_center_self,
				MapPoint(myal_game_info.info_vec_unit.at(i).n_pos));
			if (distance < distance_min) {
				distance_min = distance;
				index = i;
			}
		}
	}
	return myal_game_info.info_vec_unit.at(index).n_tag;
}

MapPoint MyAlgorithm::getKiteMapPoint(const int index) {
	//MapPoint point_sum = myal_center_self;
	//for (int i = 0; i < myal_IM_pop.size(); ++i) {
	//	if (i != index) {
	//		MapPoint point_n = myal_IM.turnGridToMap(myal_IM_pop.at(i).first);
	//		float k_n = myal_IM.calculateDistance(myal_center_self, point_n);
	//		point_sum += 2.25f / (k_n * k_n) * (myal_center_self - point_n);
	//	}
	//	else {
	//		MapPoint point_n = myal_IM.turnGridToMap(myal_IM_pop.at(i).first);
	//		float k_n = myal_IM.calculateDistance(myal_center_self, point_n);
	//		point_sum += 12.25f / (k_n * k_n) * (point_n - myal_center_self);
	//	}
	//}
	MapPoint point_enemy;
	if (index >= 0 && index < myal_IM_pop.size()) {
		point_enemy = myal_IM.turnGridToMap(myal_IM_pop.at(index).first);
	}
	else {
		point_enemy = myal_IM.turnGridToMap(myal_IM_pop.at(0).first);
	}

	MapPoint point_kite;
	if (myal_IM_pop.size() == 1) {
		Vector2D diff = myal_center_self - point_enemy;
		Normalize2D(diff);
		float distance = myal_IM.calculateDistance(myal_center_self, point_enemy);
		if (distance > 3.0f) {
			point_kite = myal_center_self - diff * distance / 3;
		}
		else {
			point_kite = myal_center_self + diff * 3.0f;
		}
	}
	else {
		//point_kite = point_enemy;
		MapPoint point_center = myal_center_enemy;
		for (int i = 0; i < myal_IM_pop.size(); ++i) {

			Vector2D diff = myal_center_self - point_center;
			Normalize2D(diff);
			point_kite = myal_center_self + diff * 3.0f;

			//MapPoint point_n = myal_IM.turnGridToMap(myal_IM_pop.at(i).first);
			//float k_n = myal_IM.calculateWeightDistance(myal_IM_pop, i, myal_center_self, point_n);
			//point_kite += 2.25f / (k_n) * (myal_center_self - point_n);

			//if (i != index) {
			//	MapPoint point_n = myal_IM.turnGridToMap(myal_IM_pop.at(i).first);
			//	float k_n = myal_IM.calculateWeightDistance(myal_IM_pop, i, myal_center_self, point_n);
			//	point_kite += 2.25f / (k_n) * (myal_center_self - point_n);
			//}
			//	else {
			//		MapPoint point_n = myal_IM.turnGridToMap(myal_IM_pop.at(i).first);
			//		float k_n = myal_IM.calculateDistance(myal_center_self, point_n);
			//		point_sum += 12.25f / (k_n * k_n) * (point_n - myal_center_self);
		}
	}

	return point_kite;
}

int MyAlgorithm::getPopIndex(const sc2::Tag tag) {
	for (int i = 0; i < myal_game_info.info_vec_unit.size(); ++i) {
		if (myal_game_info.info_vec_unit.at(i).n_tag == tag) {
			return myal_game_info.info_vec_unit.at(i).n_pop;
		}
	}
	return 0;

}
//void MyAlgorithm::Algorithm() {
//
//}

//void MyAlgorithm::end_Algorithm() {
//	game_info.info_vec_unit.clear();
//}