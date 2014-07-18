#include "pch.hpp"
#include "why_app.hpp"
#include "why_timestep.hpp"
#include "why_game_state.hpp"
#include "why_purgatory.hpp"
#include "why_utilities.hpp"

why::SettingsManager why::Application::m_settings;

why::Application::Application() : m_world(nullptr), m_rc_manager(nullptr), 
m_music_player(m_settings), m_main_menu(nullptr)
{
	using namespace boost;
	using namespace boost::log;
	std::srand(static_cast<unsigned int>(time(NULL)));

	// TODO: Fix path
	m_settings.load("../settings.json");
	
	add_file_log
		(
		keywords::file_name = m_settings.get_as_str("debug.log_file", "why.log"),
		keywords::format =
		(
			expressions::stream
			<< expressions::format_date_time< posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
			<< ": " << expressions::smessage
		)
		);

	log::add_common_attributes();

	m_console_wnd = new ConsoleWindow(window);

	WHY_LOG() << "System information:\nClanlib " << CLANLIB_VERSION_STRING
		<< "\nBoost: " << BOOST_LIB_VERSION << "\n" << get_os_info() << "\n\n";
}

why::Application::~Application()
{
	delete m_world;
	delete m_rc_manager;
	delete m_main_menu;
	delete m_console_wnd;
}

int why::Application::start ( const std::vector<std::string> &args )
{
	using namespace clan; 
	using namespace boost;

	typedef GameState gs;

	initialize();
	gs::set(GameStateValue::Menu);

	WHY_LOG() << "GAME STARTED";

	m_music_player.shuffle();
	m_music_player.play();

	Canvas canvas(window);

	ubyte64 time_elapsed_ms = 0;
	ubyte64 last_time = System::get_time();
	ubyte64 current_time = System::get_time();

	// The mighty game-loop
	while (!gs::is_active(GameStateValue::Quit))
	{
		Purgatory::send_to_hell();

		current_time = System::get_time();
		time_elapsed_ms = current_time - last_time;
		last_time = current_time;

		if ( window.get_gc() != canvas.get_gc() )
		{
			// Always get the graphic context, the window may have been recreated
			canvas = Canvas ( window );
		}
		
		draw_background(canvas);
		
		if (gs::is_active(GameStateValue::Playing))
		{
				enable_menu_events(false);
				m_world->enable_events();
				m_world->update(m_fixed_timestep, time_elapsed_ms, 5);
				m_world->draw(canvas);
				draw_score(canvas);
				draw_lives_left(canvas);
				draw_time_elapsed(canvas);
		}
		else if (gs::is_active(GameStateValue::Menu))
		{
				assert(m_main_menu);
				enable_menu_events();
				m_world->enable_events(false);
				draw_main_menu(canvas);
		}
		else if (gs::is_active(GameStateValue::GameOver))
		{
			m_world->enable_events(false);
			m_main_menu->show(MenuItemId::ResumeGame, false);
			draw_game_over(canvas);
		}
		else if (gs::is_active(GameStateValue::LevelCompleted))
		{
			m_world->enable_events(false);
			if (!m_world->load_next_level())
			{
				gs::set(GameStateValue::GameOver);
			}
			else
			{
				m_world->reset_positions();
				gs::set(GameStateValue::Playing);

			}
		}
		else if (gs::is_active(GameStateValue::Credits))
		{
			m_world->enable_events(false);
			draw_credits(canvas);
		}
		
		if (gs::is_active(GameStateValue::Paused) && !gs::is_active(GameStateValue::Menu))
		{
			m_world->enable_events(false);
			slot_mouse_clicked.enable();
			slot_input_down.enable();
			draw_game_paused(canvas);
		}
		
		if (gs::is_active(GameStateValue::Console))
		{
			m_console_wnd->draw(canvas);
			m_world->enable_events(false);
		}

		m_music_player.update();
		window.flip();
		KeepAlive::process(0);
	}
	WHY_LOG() << "Exiting game";
	return 0;
}

void why::Application::initialize()
{
	using namespace clan;

	m_resolution.width = m_settings.get_as_int("game.video.resolution_width");
	m_resolution.height = m_settings.get_as_int("game.video.resolution_height");

	create_window();

	WHY_LOG() << "Window created " << (window.is_fullscreen() ? "(FULLSCREEN)" : "(WINDOWED)");

	Canvas canvas(window);

	m_rc_manager = new ResourceManager(&window, m_settings);
	m_rc_manager->load(m_settings.get_as_str("game.paths.resources_root"));

	WHY_LOG() << "Resource manager initialized";

	m_music_player.set_soundtrack(m_rc_manager->get_soundtrack());

	m_world = new World(window, canvas, m_rc_manager, m_settings);
	m_world_area = m_world->get_world_area();

	WHY_LOG() << "World created";

	create_main_menu();
	assert(m_main_menu);

	WHY_LOG() << "Main menu created";

	init_signals();

	WHY_LOG() << "Signals connected";

	m_player_ball_sprite = m_rc_manager->get_sprite(ResourceId::PlayerBall);
	m_game_bg_sprite = m_rc_manager->get_sprite(ResourceId::BackgroundGame);
	m_menu_bg_sprite = m_rc_manager->get_sprite(ResourceId::BackgroundMenu);
}


void why::Application::init_signals()
{
	slot_wnd_closed = window.sig_window_close().connect(this, &Application::on_window_close);

	slot_input_down = window.get_ic().get_keyboard().sig_key_down().connect(this, &Application::on_input_down);

	slot_menu_mclick = window.get_ic().get_mouse().sig_pointer_move().connect(m_main_menu, &Menu::on_mouse_move);
	slot_menu_mmove = window.get_ic().get_mouse().sig_key_up().connect(this, &Application::on_menu_mouse_clicked);

	slot_mouse_clicked = window.get_ic().get_mouse().sig_key_up().connect(this, &Application::on_mouse_clicked);
}

void why::Application::create_window()
{
	using namespace clan;
	DisplayWindowDescription window_description;
	window_description.set_title("WHY");
	window_description.set_size(m_resolution, true);
	window_description.set_fullscreen(m_settings.get_as_bool("game.video.enable_fullscreen", false));
	window_description.show_caption();
	window = DisplayWindow(window_description);
}

void why::Application::create_main_menu()
{
	using namespace clan;

	typedef MenuItemId mid;
	Sprite sprite(m_rc_manager->get_sprite(ResourceId::MainMenuBackground));
	Font f(m_rc_manager->get_font(50));
	const unsigned int id(static_cast<unsigned int>(ResourceId::MainMenu));
	
	m_main_menu = new Menu(window, f, sprite, Pointf(100.0, 250.0f), id, "Main Menu");

	*m_main_menu << MenuItem(window, f, mid::ResumeGame, "Resume")
		<< MenuItem(window, f, mid::NewGame, "New Game")
		<< MenuItem(window, f, mid::LoadGame, "Load Game")
		<< MenuItem(window, f, mid::Settings, "Settings")
		<< MenuItem(window, f, mid::Credits, "Credits")
		<< MenuItem(window, f, mid::Quit, "Quit");

	m_main_menu->show(MenuItemId::ResumeGame, false);
}
const why::SettingsManager &why::Application::settings()
{
	return m_settings;
}

void why::Application::enable_menu_events(bool value)
{
	if (value)
	{
		slot_menu_mclick.enable();
		slot_menu_mmove.enable();
	}
	else
	{
		slot_menu_mclick.disable();
		slot_menu_mmove.disable();
	}
}

/////////////////////////////////////////////////
// Drawing
////////////////////////////////////////////////

void why::Application::draw_game_over(clan::Canvas &c)
{
	using namespace boost;
	using namespace std;
	using namespace clan;

	static const string go("GAME OVER");
	static const string go2("Press Any Key To Continue");
	
	static Font fbig = m_rc_manager->get_font(80);
	float x = static_cast<float>(
		((c.get_width() / 2.0f) - (fbig.get_text_size(c, go).width / 2.0f))
		);
	float y = c.get_height() / 2.0f;
	
	draw_info_box(c);
	fbig.draw_text(c, x, y, go);

	static Font fsmall = m_rc_manager->get_font();
	
	y += fbig.get_font_metrics().get_height();
	x = static_cast<float>(
		((c.get_width() / 2.0f) - (fsmall.get_text_size(c, go2).width / 2.0f))
		);

	fsmall.draw_text(c, x, y, go2);
}

void why::Application::draw_game_paused(clan::Canvas &c)
{
	using namespace clan;
	static const std::string gp1("GAME PAUSED");
	static const std::string gp2("Press Any Key To Continue");
	static Font fbig = m_rc_manager->get_font(80);
	static Font fsmall = m_rc_manager->get_font();
	
	float x = static_cast<float>(
		((c.get_width() / 2.0f) - (fbig.get_text_size(c, gp1).width / 2.0f))
		);
	float y = c.get_height() / 2.0f;
	draw_info_box(c);
	fbig.draw_text(c, x, y, gp1);
	y += fbig.get_font_metrics().get_height();
	x = static_cast<float>(
		((c.get_width() / 2.0f) - (fsmall.get_text_size(c, gp2).width / 2.0f))
	);
	fsmall.draw_text(c, x, y, gp2);
}

void why::Application::draw_info_box(clan::Canvas &c)
{
	c.fill_rect(m_world_area, clan::Colorf(0.0f, 0.0f, 0.0f, 0.6f));

	static clan:: Colorf linec(clan::Colorf::white);
	linec.set_alpha(0.3f);
	c.draw_box(m_world_area, linec);
}

void why::Application::draw_background(clan::Canvas &c, bool tile)
{
	using namespace clan;

	Sprite *bg = nullptr;
	if (GameState::is_active(GameStateValue::Menu))
	{
		bg = &m_menu_bg_sprite;
	}
	else
	{
		bg = &m_game_bg_sprite;
	}

	const float screen_width(static_cast<float>(c.get_width()));
	const float screen_height(static_cast<float>(c.get_height()));

	const float sprite_width(static_cast<float>(bg->get_width()));
	const float sprite_height(static_cast<float>(bg->get_height()));

	if (tile)
	{
		float x = 0, y = 0;

		while (y < screen_height + sprite_height)
		{
			bg->draw(c, x, y);
			x += sprite_width;
			if (x > screen_width + sprite_width)
			{
				y += sprite_height;
				x = 0.0f;
			}
		}
	}
	else
	{
		bg->draw(c, Rectf(0.0f, 0.0f, sprite_width, sprite_height),
			Rectf(0.0f, 0.0f, screen_width, screen_height));
	}
}

void why::Application::draw_score(clan::Canvas &c)
{
	using namespace clan;
	using namespace boost;
	Font &font = m_rc_manager->get_font();
	
	static format f("Score: %1%");
	static const float x = m_world_area.left + 10.0f; // px. add some margin
	static const float y = m_world_area.top - font.get_font_metrics().get_height();
	font.draw_text(c, x, y, str(f % m_world->get_total_score()));
}

void why::Application::draw_time_elapsed(clan::Canvas &c)
{
	using namespace clan;
	using namespace boost;
	Font &font = m_rc_manager->get_font();

	static format f("Time Elapsed: %1%:%2%");
	static const float x = m_world_area.left + 200.0f; // px. add some margin
	static const float y = m_world_area.top - font.get_font_metrics().get_height();
	auto te = m_world->get_level_time_elapsed();
	font.draw_text(c, x, y, str(f % te.get<1>() % te.get<2>()));
}

void why::Application::draw_lives_left(clan::Canvas &c)
{
	using namespace clan;

	if (m_world->get_player_lives() == 0)
	{
		return;
	}

	const unsigned int player_lives(m_world->get_player_lives());
	const unsigned int ball_width(m_player_ball_sprite.get_width());
	const unsigned int ball_height(m_player_ball_sprite.get_height());
	float start_x = m_world_area.right - ((ball_width*player_lives) + ((ball_width*player_lives) / player_lives));
	const float y = m_world_area.top - m_player_ball_sprite.get_height();
	
	for (int bidx = m_world->get_player_lives(); bidx > 0; --bidx)
	{
		m_player_ball_sprite.draw(c, Rectf(start_x, y, start_x + ball_width, y - ball_height));
		start_x += ball_width + (ball_width / player_lives);
	}
}

void why::Application::draw_main_menu(clan::Canvas &c)
{
	using namespace clan;
	m_main_menu->draw(c);

	const std::string ver (std::string(INERTIA_VERSION) + "v");
	Font f = m_rc_manager->get_font();

	Pointf vpos;
	vpos.x = c.get_width() - (ver.length() * f.get_font_metrics().get_average_character_width() + 10.0f);
	vpos.y = c.get_height() - 10.0f;

	f.draw_text(c, vpos, ver);
}

void why::Application::draw_credits(clan::Canvas &c)
{
	using namespace boost;
	using namespace clan;
	using namespace std;

	draw_info_box(c);

	Font font_norm = m_rc_manager->get_font(30);
	Font font_big = m_rc_manager->get_font(75);

	auto height_norm = font_norm.get_font_metrics().get_height();
	Pointf pos = Pointf(font_norm.get_font_metrics().get_average_character_width() * 3, 
		m_world_area.top + font_big.get_font_metrics().get_height() * 1.5f);

	font_big.draw_text(c, pos, "Credits");
	pos.y += font_big.get_font_metrics().get_height() * 1.1f;

	font_norm.draw_text(c, pos, "Coding and Graphics by Ville Lindell\n\n"\
		"Music by FoxSynergy, HorrorPen, Maxstack, professorlamp and Thomas Bruno.\nCheck them out at OpenGameArt.org!");
}

///////////////////////////////////////////////////////
// Event handlers
///////////////////////////////////////////////////////

void why::Application::on_input_down(const clan::InputEvent &key)
{
	typedef GameState gs;

	if (gs::is_active(GameStateValue::GameOver))
	{
		m_main_menu->show(MenuItemId::ResumeGame, false);
		GameState::set(GameStateValue::Menu);
		return;
	}

	if (key.id == clan::keycode_escape && key.shift)
	{
		if (GameState::is_active(GameStateValue::Console))
		{
			m_console_wnd->show(false);
			gs::remove(GameStateValue::Console);
		}
		else
		{
			m_console_wnd->show(true);
			gs::add(GameStateValue::Console);
			gs::add(GameStateValue::Paused);
			if (!gs::is_active(GameStateValue::Menu))
			{
				m_world->enable_events(false);
				m_world->pause();
			}
		}
		return;
	}

	if (key.id == clan::keycode_escape && gs::is_active(GameStateValue::Playing))
	{
		m_world->pause();
		m_main_menu->show(MenuItemId::ResumeGame);
		gs::set(GameStateValue::Menu);
		gs::add(GameStateValue::Paused);
		
	}
	else if (key.str == "," && !gs::is_active(GameStateValue::Menu))
	{
		m_music_player.stop();
		m_music_player.previous();
		m_music_player.play();
	}
	else if (key.str == "." && !gs::is_active(GameStateValue::Menu))
	{
		m_music_player.stop();
		m_music_player.next();
		m_music_player.play();
	}
}

void why::Application::on_mouse_clicked(const clan::InputEvent &e)
{
	if (GameState::is_active(GameStateValue::GameOver))
	{
		GameState::set(GameStateValue::Menu);
	}
	else if (GameState::is_active(GameStateValue::Paused))
	{
		GameState::set(GameStateValue::Playing);
		m_world->resume();
	}
}

void why::Application::on_window_close()
{
	GameState::set(GameStateValue::Quit);
}

void why::Application::on_menu_mouse_clicked(const clan::InputEvent &e)
{
	assert(m_main_menu);
	const MenuItem &si = m_main_menu->get_selection();

	// No selection?
	if (si.is_invalid())
	{
		return;
	}
	
	switch (si.get_id())
	{
		case MenuItemId::ResumeGame:
		{
			m_music_player.stop();
			slot_mouse_clicked.disable();
			slot_input_down.disable();
			GameState::set(GameStateValue::Paused);
			m_music_player.play();
			break;
		}
		case MenuItemId::NewGame:
		{
			m_main_menu->show(MenuItemId::ResumeGame, false);
			m_music_player.stop();
			m_world->kill_player();
			m_world->reset_score();
			
			try
			{
				m_world->load_level_index();
			}
			catch (std::exception &e)
			{
				WHY_LOG() << e.what();
				return;
			}
			m_world->spawn_player();
			m_world->load_next_level();
			
			GameState::set(GameStateValue::Playing);
			m_music_player.play();
			break;
		}
		case MenuItemId::Quit:
		{
			GameState::set(GameStateValue::Quit);
			break;
		}
		case MenuItemId::Credits:
		{
			GameState::set(GameStateValue::Credits);
			break;
		}
		default:
		{
			break;
		}
	}
}