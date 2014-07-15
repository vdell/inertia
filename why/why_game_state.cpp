#include "pch.hpp"
#include "why_game_state.hpp"

why::GameStateValueSet why::GameState::m_states;

void why::GameState::set(GameStateValue s)
{
	m_states.clear();
	m_states.insert(s);
}

bool why::GameState::is_active(GameStateValue s)
{
	if (m_states.empty()) return false;
	return m_states.find(s) != m_states.end();
}

void why::GameState::add(GameStateValue s)
{
	auto ret = m_states.insert(s);
	assert(ret.second);
}

void why::GameState::remove(GameStateValue s)
{
	m_states.erase(s);
}

/////////////////////////////////////////
// Free Functions
/////////////////////////////////////////

std::string why::gsv2str(GameStateValue s)
{
	return std::string();
}