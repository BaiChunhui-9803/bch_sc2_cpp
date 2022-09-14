#ifndef STATE_H
#define STATE_H
#include<sc2api/sc2_api.h>
//#include "action_order.h"
#include "Command.h"
#include "Solution.h"

#define SimulateSize 100				//一个客户端模拟解的个数

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
		UnitTypeID m_unit_type;
		Point2D m_pos;
		uint32_t m_player_id = 0;
		Tag m_unit_tag = 0;
		float m_energy = 0;
		float m_life = 0;
		float m_shields = 0;

		UnitState() = default;
		UnitState(UnitTypeID unit_type, Point2D pos, uint32_t player_id = 0, Tag unit_tag = 0, float energy = 0, float life = 0, float shields = 0) :
			m_unit_type(unit_type), m_pos(pos), m_player_id(player_id), m_unit_tag(unit_tag), m_energy(energy), m_life(life), m_shields(shields) {}
	};

	struct State {
		std::vector<UnitState> m_units_state;
		//一个客户端模拟多个命令，因此是vector
		std::vector<Command> m_commands;

		State() {
			m_units_state = std::vector<UnitState>();
			m_commands = std::vector<Command>();
			//m_commands.resize(SimulateSize);
		}
		State(std::vector<UnitState> unit_state_vec) :m_units_state(unit_state_vec) {}
		State SaveState(const ObservationInterface* observation);

        friend std::ostream& operator<<(std::ostream& os, const State& s);
        void LoadState(State saved_state, Client& current_client, Coordinator& current_coordinator, std::vector<Solution> load_solutions);
		bool isBlank() { return (m_units_state.size() == 0); }
		Point2D getCenterPos();
	};

    struct GameInfCallFor {
        double g_inf_score = 0;
        //ActionOrder g_inf_action_order;
    };


}

#endif