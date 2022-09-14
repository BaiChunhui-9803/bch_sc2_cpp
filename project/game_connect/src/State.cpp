#include "State.h"
#include <sc2utils/sc2_manage_process.h>

using namespace sc2;

State sc2::State::SaveState(const ObservationInterface* observation) {
	State save_state;
	Units units = observation->GetUnits();
	save_state.m_units_state.resize(units.size());
	const Unit* u;
	for (size_t i = 0; i < units.size(); ++i) {
		u = units.at(i);
		save_state.m_units_state[i] = UnitState({
			u->unit_type,
			u->pos,
			static_cast<uint32_t>(u->owner),
			u->tag,
			u->energy,
			u->health,
			u->shield
			});
	}
	return save_state;
}

void sc2::State::LoadState(State saved_state, Client& current_client, Coordinator& current_coordinator, std::vector<Solution> load_solutions) {

	*this = saved_state;
	for (int i = 0; i < load_solutions.size(); ++i) {
		this->m_commands.push_back(load_solutions[i].s_commands);

	}
	if (!saved_state.isBlank()) {
		// 杀死所有现有单位
		for (const Unit* u : current_client.Observation()->GetUnits()) {
			current_client.Debug()->DebugKillUnit(u);
		}
		current_client.Debug()->SendDebug();
		// 残骸清除大约需要20个Loop
		for (size_t i = 0; i < 20; ++i) {
			current_coordinator.Update();
		}
		// 从saved_state中创建Units
		for (UnitState u : saved_state.m_units_state) {
			current_client.Debug()->DebugCreateUnit(u.m_unit_type, u.m_pos, u.m_player_id);
		}
		current_client.Debug()->SendDebug();
		// DebugCreateUnit()至少需要2个Loop
		for (size_t i = 0; i < 2; ++i) {
			current_coordinator.Update();
		}
		// 复制saved_state中的属性
		const Unit* copy_unit;
		Units copy_units = current_client.Observation()->GetUnits();
		for (UnitState u : saved_state.m_units_state) {
			copy_unit = select_nearest_unit_from_point(u.m_pos, copy_units);
			current_client.Debug()->DebugSetShields(u.m_shields + 0.1f, copy_unit);
			current_client.Debug()->DebugSetLife(u.m_life, copy_unit);
			//std::cout << "current_client life:" << u.life << std::endl;
			current_client.Debug()->DebugSetEnergy(u.m_energy, copy_unit);
		}
		current_client.Debug()->SendDebug();
		// DebugCreateUnit()至少需要2个Loop
		for (size_t i = 0; i < 2; ++i) {
			current_coordinator.Update();
		}
	}
}

Point2D sc2::State::getCenterPos() {
	Point2D center_pos;
	for (auto& u : this->m_units_state) {
		center_pos += u.m_pos;
	}
	return center_pos / this->m_units_state.size();
}

std::ostream& sc2::operator<<(std::ostream& os, const State& s) {
	os << "输出State_Units" << std::endl;
	os << "id\ttag\t\ttype\tplayer\tlife\tenergy\tpos.x\tpos.y" << std::endl;
	for (size_t i = 0; i < s.m_units_state.size(); ++i) {
		os << i << "\t"
			<< s.m_units_state.at(i).m_unit_tag << "\t"
			<< s.m_units_state.at(i).m_unit_type << "\t"
			<< s.m_units_state.at(i).m_player_id << "\t"
			<< s.m_units_state.at(i).m_life << "\t"
			<< s.m_units_state.at(i).m_energy << "\t"
			<< s.m_units_state.at(i).m_pos.x << "\t"
			<< s.m_units_state.at(i).m_pos.y << std::endl;
	}
	return os;
}