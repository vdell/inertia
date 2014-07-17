#pragma once

#include "why_game_object.hpp"

namespace why
{
	/*! Purgatory of game objects 
	 *
	 * @detail Purgatory is the place where deleted game objects are sent if they are
	 *         deleted during PhysicsWorld::step. The main game-loop then calls 
	 *         Purgatory::send_to_hell before the next step. If the objects would be deleted directly
	 *         inside the step then the game could experience random crashes that are caused by PhysicsWorld
	 *         not knowing that the object is gone.
	 **/
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