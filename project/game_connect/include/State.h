#pragma once
#include<sc2api/sc2_api.h>
#include "action_order.h"

namespace sc2 {

	////////// 公共函数部分 /////////
	static const Unit* select_nearest_unit_from_point(const Point2D& p, const Units& us) {
		float min_distance = FLT_MAX;
		const Unit* selected_unit = nullptr;
		float dis;
		for (const auto u : us) {
			dis = Distance2D(p, u->pos);
			if (dis < min_distance) {
				selected_unit = u;
				min_distance = dis;
			}
		}
		return selected_unit;
	}
	////////////////////////////////

	struct UnitState {
		UnitTypeID unit_type;
		Point2D pos;
		uint32_t player_id = 0;
		Tag unit_tag = 0;
		float energy = 0;
		float life = 0;
		float shields = 0;
	};

	struct State {
		std::vector<UnitState> m_units_state;

        friend std::ostream& operator<<(std::ostream& os, const State& s);

        State SaveState(const ObservationInterface* observation);

        void LoadState(State saved_state, Client& current_client, Coordinator& current_coordinator);

		bool isBlank() { return (m_units_state.size() == 0); }
	};

    struct GameInfCallFor {
        double g_inf_score = 0;
        ActionOrder g_inf_action_order;
    };


}
