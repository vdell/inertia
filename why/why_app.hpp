#pragma once

#include "why_world.hpp"
#include "why_resource_manager.hpp"
#include "why_settings_manager.hpp"
#include "why_music_player.hpp"
#include "why_menu.hpp"
#include "why_console.hpp"

namespace why
{
	//! Menu item IDs 
	enum MenuItemId
	{
		ResumeGame, NewGame, LoadGame, Settings, Credits, Quit
	};

	/*! @brief The application class
	 *
	 * @detail This class represents the application itself. It takes care of the main loop, 
	 *         (some of the) user input, event handling and such.
	 **/
	class Application	
	{
	public:
		//! Constructor
		Application();

		//! Destructor
		~Application();

		/* @brief Starts the application
		 *
		 * @param args Commandline arguments 
		 * @return Exit code
		 **/
		int start ( const std::vector<std::string> &args );

		//! Returns application settings manager.
		static const SettingsManager &settings();
	private:
		void on_input_down(const clan::InputEvent &key);
		void on_con_input_down(const clan::InputEvent &key);
		void on_menu_mouse_clicked(const clan::InputEvent &e);
		void on_mouse_clicked(const clan::InputEvent &e);
		void on_window_close();

		void draw_score(clan::Canvas &c);
		void draw_time_elapsed(clan::Canvas &c);
		void draw_lives_left(clan::Canvas &c);
		void draw_background(clan::Canvas &c, bool tile = false);
		void draw_game_over(clan::Canvas &c);
		void draw_game_paused(clan::Canvas &c);
		void draw_main_menu(clan::Canvas &c);
		void draw_credits(clan::Canvas &c);

		void draw_info_box(clan::Canvas &c);

		void enable_menu_events(bool value = true);

		void initialize();

		void create_window();
		void create_main_menu();
		void init_signals();

		clan::DisplayWindow window;
		ConsoleWindow *m_console_wnd;
	
		clan::Slot slot_input_down, slot_con_input_down, slot_window_resize, 
			slot_menu_mclick, slot_menu_mmove, slot_mouse_clicked, slot_wnd_closed;
		
		MusicPlayer m_music_player;

		const float m_fixed_timestep = 1.0f / 60.0f;

		static SettingsManager m_settings;
		ResourceManager *m_rc_manager;

		World *m_world;
		clan::Size m_resolution;
		clan::Rectf m_world_area;
		clan::Sprite m_player_ball_sprite, m_game_bg_sprite, m_menu_bg_sprite;

		std::multimap < std::string, std::string > m_credits;
		Menu *m_main_menu;
	};

}