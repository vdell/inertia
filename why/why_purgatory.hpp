#pragma once

#include "why_game_object.hpp"

namespace why
{
	class Purgatory
	{
	public:
		static void add(GameObjectBase *o);
		static bool is_added(const GameObjectBase *o);
		static void send_to_hell();
	private:
		static std::deque < GameObjectBase * > m_purgatory;
	};
}