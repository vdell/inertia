#pragma once

#include "why_level.hpp"
#include "why_ball_object.hpp"
#include "why_paddle_object.hpp"
#include "why_resource_manager.hpp"
#include "why_timestep.hpp"
#include "why_settings_manager.hpp"
#include "why_effect.hpp"

namespace why
{
	/* @brief The game world class
	 *
	 * @detail This class takes care of all the things happening in the game world, like e.g. loading levels,
	 *         updating objects and keeping record of the players' score / level time.
	 **/
	class World
	{
	public:
		/*! @brief Constructor
		 *
		 * @param parent Reference to the parent window
		 * @param c Reference to the canvas that the world will be drawn
		 * @param rc_mngr Pointer to the resource manager
		 * @param Reference to the settings manager
		 */
		World(clan::DisplayWindow &parent, clan::Canvas &c,
			ResourceManager *rc_mngr, const SettingsManager &sm);
		
		//! Destructor
		~World();

		void spawn_player();
		void kill_player();
		bool is_player_alive() const;

		void pause();
		void resume();

		unsigned int get_total_score() const;
		unsigned int get_player_lives() const;

		/*! @brief Loads JSON-based level index file
		 *
		 * @detail The file is expected to be located in the root of the levels directory 
		 *          and the filename must be index.json. 
		 *
		 * @bug If the JSON-file is malformed the loader will throw an exception and the 
		 *      game crashes.
		 **/
		void load_level_index();

		//! Loads next level, returing true if there are levels to load or false otherwise.
		bool load_next_level();

		void enable_events(bool value = true);

		//! Resets the total score to zero
		void reset_score();

		void update(float fixed_timestep, clan::ubyte64 time_elapsed_ms, int max_steps);
		void draw(clan::Canvas &canvas);

		//! Returns a tuple containing the elapsed level time (t[0] = hours, t[1] = minutes and t[2] = seconds)
		boost::tuple<int, int, int> get_level_time_elapsed() const;

		void reset_positions();

		/* @brief Returns the world area (i.e. the space for the blocks, paddle and balls)
		*
		* @param c The canvas to which the world is drawn
		**/
		clan::Rectf get_world_area(const clan::Canvas &c) const;
	private:
		void initialize(clan::DisplayWindow &parent_wnd, clan::Canvas &c);
		void create_area();
		void create_physics();

		void on_mouse_move(const clan::InputEvent &evt);
		void on_mouse_click(const clan::InputEvent &evt);
		void on_kbd_up(const clan::InputEvent &evt);
		void on_kbd_down(const clan::InputEvent &evt);

		void draw_dbg(clan::Canvas &c);

		unsigned int kill_ball(bool do_death_flash_effect = true);
		bool is_ball_lost() const;

		void smooth_states();
		void reset_smooth_states();

		// Spawns a new bubble object.
		void spawn_bubble();

		/*! @brief Kills bubbles
		 *
		 * @param kill_all Should all bubble objects be killed (true) or just the ones below the game area (false)
		 */
		void kill_bubbles(bool kill_all = false);

		void add_random_mod_to_paddle();

		GameObjectBasePtrDeque m_objects;
		void add_object(GameObjectBase *obj);

		std::deque < EffectBase * > m_active_effects;

		const SettingsManager &m_settings;
		ResourceManager *m_rc_manager;

		std::stack<std::string> m_level_index;

		Level *m_level;

		BallObject *m_ball;
		PaddleObject *m_paddle;

		bool m_events_enabled;

		clan::Rectf m_area;
		clan::Colorf m_area_color, m_line_color;
		clan::Canvas *m_canvas;

		clan::Slot slot_mouse_move;
		clan::Slot slot_mouse_click;
		clan::Slot slot_input_down, slot_input_up;

		unsigned int m_player_lives, m_total_score;

		float m_fixed_timestep_accumulator, 
			m_fixed_timestep_accumulator_ratio;

		clan::PhysicsWorld m_pworld;

		clan::PhysicsDebugDraw *m_dbg_draw;
	};
}