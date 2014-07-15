#pragma once

namespace why
{
	//! Enum class of valid game state values (which should be self-explanatory)
	enum class GameStateValue
	{
		Playing, GameOver, Menu, LevelCompleted, Paused, Console, Quit, Credits
	};

	std::string gsv2str(GameStateValue s);

	//! Container type for storing game state values
	typedef std::set<GameStateValue> GameStateValueSet;

	/*! @brief A class that manages the game state values
	 *
	 * @detail There can be multiple values active at the same time. At the moment this 
	 *         class doesn't check if the value combinations make sense (i.e. GameStateValue::Paused
	 *         and GameStateValue::Playing can appear at the same time)
	 **/
	class GameState
	{
	public:
		//! Adds the given state as an active state.
		static void add(GameStateValue s);

		//! Clears all states and makes the given state as the only active one.
		static void set(GameStateValue s);

		//! Returns true if the given state is active, false otherwise.
		static bool is_active(GameStateValue s);

		//! Removes the given state if it exists.
		static void remove(GameStateValue s);
	private:
		static GameStateValueSet m_states;
	};
}