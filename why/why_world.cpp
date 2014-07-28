#include "pch.hpp"
#include "why_world.hpp"
#include "why_game_state.hpp"
#include "why_purgatory.hpp"
#include "why_utilities.hpp"
#include "why_bubble_object.hpp"
#include "why_game_object_modifier.hpp"

//////////////////////////////////////////////////
// World
//////////////////////////////////////////////////

why::World::World(clan::DisplayWindow &parent, clan::Canvas &c, ResourceManager *rc_mngr, 
	const SettingsManager &sm) : m_rc_manager(rc_mngr), m_settings(sm), 
	m_paddle_angle_mod(0)
{
	assert(m_rc_manager);
	
	m_level = nullptr;
	
	m_parent = &parent;
	m_canvas = &c;

	m_fixed_timestep_accumulator = 0;
	m_fixed_timestep_accumulator_ratio = 0;
	m_player_lives = 0;
	m_total_score = 0;

	m_events_enabled = false;
	m_paused = false;

	m_line_color = clan::Colorf::white;
	m_line_color.set_alpha(0.3f);

	m_prev_mouse_pos = clan::Point(-1, -1);

	initialize();
}

why::World::~World()
{
	delete m_level;
	for (auto o : m_objects)
	{
		if (o)
		{
			// The modifiers would be deleted automatically by the game object base destructor but this has
			// the potential for a crash if the modifier tries to dynamic cast the object inside ModifierXXX::reset().
			o->delete_modifiers();
			delete o;
		}
	}
}

void why::World::pause()
{
	m_paused = true;
	enable_events(false);
	m_ball_linear_velocity = m_ball->body().get_linear_velocity();
	m_level->pause();

	WHY_LOG() << "Game paused";
}

void why::World::resume()
{
	m_paused = false;
	enable_events();
	m_ball->body().set_linear_velocity(m_ball_linear_velocity);
	m_level->start();

	WHY_LOG() << "Game resumed";
}

void why::World::initialize()
{
	m_box2d_debug_draw_enabled = m_settings.get_as_bool("debug.enable_box2d_debug_draw", false);

	m_paddle_btm_margin = m_settings.get_as_float("game.core.paddle_btm_margin_perc", 0.05f);

	m_area = get_world_area();

	m_area_color = clan::Colorf(m_settings.get_as_str("game.core.game_area_color_hex", "#000913"));
	m_area_color.set_alpha(0.7f);

	load_level_index();
	create_physics();
	create_area();

	m_ball = new BallObject(-1, m_canvas, m_rc_manager->get_sprite(ResourceId::PlayerBall),
		m_pworld.get_pc(), 1, m_settings);
	add_object(m_ball);
	m_paddle = new PaddleObject(-1, m_canvas, m_rc_manager->get_sprite(ResourceId::PlayerPaddle), 
		m_pworld.get_pc(), m_settings);
	add_object(m_paddle);

	

	slot_mouse_move = m_parent->get_ic().get_mouse().sig_pointer_move().connect(this, &World::on_mouse_move);
	slot_mouse_click = m_parent->get_ic().get_mouse().sig_key_up().connect(this, &World::on_mouse_click);
	slot_input_down = m_parent->get_ic().get_keyboard().sig_key_down().connect(this, &World::on_kbd_down);
	slot_input_up = m_parent->get_ic().get_keyboard().sig_key_up().connect(this, &World::on_kbd_up);
}

void why::World::load_level_index()
{
	using namespace boost::property_tree;

	WHY_LOG() << "Loading level index...";

	while (!m_level_index.empty())
	{
		m_level_index.pop();
	}

	const std::string path(m_settings.get_as_str("game.paths.resources_root") +
		m_settings.get_as_str("game.paths.levels") + "index.json");

	assert(!path.empty());
	ptree pt;
	read_json(path, pt);
	assert(!pt.empty());

	for (auto lnode : boost::adaptors::reverse(pt.get_child("levels")))
	{
		m_level_index.push(lnode.second.data());
	}

	WHY_LOG() << m_level_index.size() << " level(s) loaded";
}

bool why::World::load_next_level()
{
	assert(is_player_alive());
	Purgatory::send_to_hell();
	slot_mouse_move.disable();
	slot_mouse_click.disable();
	m_ball->stop_movement();

	if (m_level)
	{
		m_total_score += m_level->get_score();
		delete m_level;
	}
	if (m_level_index.empty())
	{
		return false;
	}

	const std::string path(m_settings.get_as_str("game.paths.resources_root") +
		m_settings.get_as_str("game.paths.levels") + m_level_index.top());
	m_level = new Level(m_area);
	m_level->load(path, *m_canvas, *m_rc_manager, m_pworld.get_pc(), m_settings);
	m_level_index.pop();
	//reset_positions();
	return true;
}

void why::World::create_area()
{
	using namespace clan;

	const float wall_thickness = 5.0f;
	const float area_width = m_area.get_width();

	//////////////////////////////////////////////
	// Ground
	//////////////////////////////////////////////

	/*
	BodyDescription ground_desc(m_pworld);
	ground_desc.set_position(Pointf(m_area.left + m_area.get_width() / 2.0f, m_area.bottom + wall_thickness/2.0f));
	ground_desc.set_type(body_static);

	Body ground(m_pworld.get_pc(), ground_desc);

	Fixture ground_fixture(m_pworld.get_pc(), ground, gf_fixture_desc);
	*/

	// Ground/floor shape
	PolygonShape gf_shape(m_pworld);
	gf_shape.set_as_box(area_width / 2.0f, wall_thickness);

	// Ground/floor fixture
	FixtureDescription gf_fixture_desc(m_pworld);
	gf_fixture_desc.set_friction(0.0f);
	gf_fixture_desc.set_shape(gf_shape);
	gf_fixture_desc.set_density(1.0f);
	gf_fixture_desc.set_restitution(1.0f);

	//////////////////////////////////////////////
	// Roof
	//////////////////////////////////////////////

	BodyDescription roof_desc(m_pworld);
	roof_desc.set_position(Pointf(m_area.left + area_width / 2.0f, m_area.top - wall_thickness / 2.0f));
	roof_desc.set_type(body_static);

	Body roof(m_pworld.get_pc(), roof_desc);
	Fixture roof_fixture(m_pworld.get_pc(), roof, gf_fixture_desc);

	//////////////////////////////////////////////
	// Walls
	//////////////////////////////////////////////

	// Left

	BodyDescription leftw_desc(m_pworld);
	leftw_desc.set_position(Pointf(m_area.left - wall_thickness / 2.0f, m_area.top + m_area.get_height() / 2.0f));
	leftw_desc.set_type(body_static);

	Body leftw(m_pworld.get_pc(), leftw_desc);

	PolygonShape wall_shape(m_pworld);
	wall_shape.set_as_box(wall_thickness, m_area.get_height() / 2.0f);

	FixtureDescription wall_fixture_desc(m_pworld);
	wall_fixture_desc.set_shape(wall_shape);
	wall_fixture_desc.set_density(1.0f);
	wall_fixture_desc.set_friction(0.0f);
	wall_fixture_desc.set_restitution(1.0f);

	Fixture leftw_fixture(m_pworld.get_pc(), leftw, wall_fixture_desc);

	// Right

	BodyDescription rightw_desc(m_pworld);
	rightw_desc.set_position(Pointf(m_area.right - wall_thickness / 2, m_area.top + m_area.get_height() / 2.0f));
	rightw_desc.set_type(body_static);

	Body rightw(m_pworld.get_pc(), rightw_desc);

	Fixture right_fixture(m_pworld.get_pc(), rightw, wall_fixture_desc);
}

void why::World::create_physics()
{
	using namespace clan;

	PhysicsWorldDescription pwd;
	pwd.set_gravity(m_settings.get_as_float("game.physics.gravity_x", 0.0f), 
		m_settings.get_as_float("game.physics.gravity_y", -9.8f));
	pwd.set_sleep(true);
	pwd.set_physic_scale(m_settings.get_as_int("game.physics.physic_scale", 30));
	m_pworld = PhysicsWorld(pwd);
	m_pworld.set_auto_clear_forces(false);

	if (m_box2d_debug_draw_enabled)
	{
		m_dbg_draw = PhysicsDebugDraw(m_pworld);
		m_dbg_draw.set_flags(f_shape | f_aabb);
	}
}

void why::World::add_object(GameObjectBase *obj)
{
	assert(obj);
	m_objects.push_back(obj);
}

boost::tuple<int, int, int> why::World::get_level_time_elapsed() const
{
	if (m_level)
	{
		return m_level->get_time_elapsed();
	}
	return boost::make_tuple(0, 0, 0);
}

unsigned int why::World::get_total_score() const
{
	if (GameState::is_active(GameStateValue::Playing))
	{
		return m_total_score + m_level->get_score();
	}
	else
	{
		return m_total_score;
	}
}

bool why::World::is_player_alive() const
{
	return get_player_lives() > 0;
}

void why::World::reset_score()
{
	m_total_score = 0;
	if(m_level)
		m_level->reset_score();
}

bool why::World::is_ball_lost() const
{
	assert(is_player_alive());
	if (!m_ball->is_moving())
	{
		return false;
	}

	return (m_ball->get_position().y > m_area.bottom);
}

void why::World::update(float fixed_timestep, clan::ubyte64 time_elapsed_ms, int max_steps)
{
	using namespace clan;
	using namespace std;

	if (!GameState::is_active(GameStateValue::Playing))
	{
		return;
	}

	m_fixed_timestep_accumulator += time_elapsed_ms;
	const int steps (Real2Int(m_fixed_timestep_accumulator / fixed_timestep));

	if (steps > 0)
	{
		m_fixed_timestep_accumulator -= steps * fixed_timestep;
	}

	m_fixed_timestep_accumulator_ratio = m_fixed_timestep_accumulator / fixed_timestep;

	const int steps_clamped = std::min(steps, max_steps);

	for (int i = 0; i < steps_clamped; ++i)
	{
		reset_smooth_states();
		m_pworld.step(fixed_timestep);
		m_level->update(time_elapsed_ms);
		m_paddle->update(time_elapsed_ms);
		m_ball->update(time_elapsed_ms);
	}

	m_pworld.clear_forces();
	smooth_states();

		
	if (is_ball_lost())
	{
		if (kill_ball() == 0)
		{
			kill_bubbles(true);
			GameState::set(GameStateValue::GameOver);
		}
		else
		{
			reset_positions();
			m_level->pause();
		}

		for (auto &object : m_objects)
		{
			object->delete_modifiers();
		}
	}
	else if (m_level->is_completed())
	{
		GameState::set(GameStateValue::LevelCompleted);
		kill_bubbles(true);

		for (auto &object : m_objects)
		{
			object->delete_modifiers();
		}
	}
	else
	{
		kill_bubbles(false);

		static const int req_combo_count_for_bubbles = 2;

		// Should we spawn a new bubble?
		if (m_level->get_combo_count() >= req_combo_count_for_bubbles)
		{
			spawn_bubble();
			m_level->reset_combo_count();
		}

		for (auto object : m_objects)
		{
			auto bo = dynamic_cast<BubbleObject*>(object);
			if (bo && bo->has_collided_with_paddle())
			{

			}
		}
	}
}

void why::World::spawn_bubble()
{
	// The memory is released when the bubble is below the game area or when the level is completed.
	auto b = new BubbleObject(-1, m_canvas, m_rc_manager->get_sprite(ResourceId::Bubble), m_pworld.get_pc(), m_settings);
	b->set_position(clan::Vec2f(m_area.get_center().x, m_area.top + b->get_height()));
	add_object(b);
}

void why::World::kill_bubbles(bool kill_all)
{
	auto it = m_objects.begin();

	if (kill_all)
	{
		while (it != m_objects.end())
		{
			if (dynamic_cast<const BubbleObject*>(*it))
			{
				Purgatory::add(*it);
				it = m_objects.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	else
	{
		while (it != m_objects.end())
		{
			auto bubble_o = dynamic_cast<BubbleObject*>(*it);
			if (bubble_o && bubble_o->get_position().y > m_area.bottom)
			{
				Purgatory::add(bubble_o);
				it = m_objects.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void why::World::enable_events(bool value)
{
	if (!m_events_enabled && value)
	{
		slot_mouse_click.enable();
		slot_mouse_move.enable();
		slot_input_down.enable();
		slot_input_up.enable();
		m_events_enabled = true;

		WHY_LOG() << "World events enabled";
	}
	else if (m_events_enabled && !value)
	{
		slot_mouse_click.disable();
		slot_mouse_move.disable();
		slot_input_down.disable();
		slot_input_up.disable();
		m_events_enabled = false;

		WHY_LOG() << "World events disabled";
	}
}

void why::World::reset_smooth_states()
{
	for (auto &o : m_objects)
	{
		MovingObject *mo = dynamic_cast<MovingObject*>(o);
		if (mo)
		{
			mo->set_smoothed_position(mo->get_prev_position());
			mo->set_prev_position(mo->get_position());

			mo->set_smoothed_angle(mo->get_prev_angle());
			mo->set_prev_angle(mo->get_rotation().to_radians());
		}
	}
}

void why::World::smooth_states()
{
	const float one_minus_ratio = 1.0f - m_fixed_timestep_accumulator_ratio;

	for (auto &o : m_objects)
	{
		MovingObject *mo = dynamic_cast<MovingObject*>(o);
		if (mo)
		{
			const clan::Vec2f bpos(mo->get_position());

			mo->set_smoothed_position(m_fixed_timestep_accumulator_ratio * bpos +
				one_minus_ratio * mo->get_prev_position());
			mo->set_prev_position(bpos);

			mo->set_smoothed_angle(m_fixed_timestep_accumulator_ratio * mo->get_rotation().to_degrees() +
				one_minus_ratio * mo->get_prev_angle());
		}
	}
}

unsigned int why::World::kill_ball()
{
	using namespace clan;
	assert(m_player_lives > 0);
	--m_player_lives;
	m_paddle->stop_movement();
	m_ball->stop_movement();
	m_prev_mouse_pos = Point(-1, -1);
	m_ball_linear_velocity = Vec2f(0.0f, 0.0f);
	return m_player_lives;
}

void why::World::kill_player()
{
	while (get_player_lives() > 0)
	{
		kill_ball();
	}
}


void why::World::draw(clan::Canvas &canvas)
{
	using namespace clan;

	canvas.fill_rect(m_area, m_area_color);

	if (GameState::is_active(GameStateValue::Playing))
	{
		m_level->draw(canvas);

		for (auto &o : m_objects)
		{
			o->draw(canvas);
		}

#ifdef _DEBUG
		draw_dbg(canvas);
#endif

		// Box2D debug draw can also be used in release builds.

		if (m_box2d_debug_draw_enabled)
		{
			m_dbg_draw.draw(canvas);
		}
	}

	canvas.draw_box(m_area, m_line_color);
}

void why::World::draw_dbg(clan::Canvas &c)
{
	using namespace clan;
	using namespace boost;
#ifdef _DEBUG
	Font f(m_rc_manager->get_font());

	const Vec2f ball_lv(m_ball->body().get_linear_velocity());
	f.draw_text(c, Pointf(m_area.left + 50, m_area.top + 50), str(format("Ball lin. velocity: %1%, %2%") % ball_lv.x % ball_lv.y));
#endif
}

unsigned int why::World::get_player_lives() const
{
	return m_player_lives;
}

void why::World::spawn_player()
{
	assert(!is_player_alive());
	reset_positions();
	m_player_lives = m_settings.get_as_int("game.defaults.player_lives", 3);
	m_prev_mouse_pos = clan::Point(-1, -1);
}

void why::World::reset_positions()
{
	using namespace clan;

	if (!m_paddle || !m_ball)
	{
		return;
	}

	PaddleObject *p = m_paddle;
	const float paddle_width = p->get_width();
	const float paddle_height = p->get_height();
	const float paddle_middle_x = paddle_width / 2.0f;

	const float paddle_btm_margin(m_area.get_height() * m_settings.get_as_float("game.core.paddle_btm_margin_perc", 0.05f));
	
	const Pointf pp
		(m_area.get_center().x - paddle_middle_x,
		((m_area.bottom - (paddle_height + paddle_btm_margin))));
	
	p->set_position(pp);
	p->set_rotation(Angle(0, AngleUnit::angle_degrees));

	const Pointf bp(pp.x + (paddle_middle_x - (m_ball->get_width() / 2.0f)), pp.y - paddle_height - 1.0f);
	m_ball->set_position(bp);
}

clan::Rectf why::World::get_world_area() const
{
	const float width = m_settings.get_as_float("game.core.game_area_width");
	const float height = m_settings.get_as_float("game.core.game_area_height");

	const float min_x ( (m_canvas->get_width() - width) / 2.0f);
	const float max_x ( min_x + width );
	const float min_y((m_canvas->get_height() - height) / 2.0f);
	const float max_y ( min_y + height );
	return clan::Rectf (min_x, min_y, max_x, max_y);
}

///////////////////////////////////////////////////
// Event handlers
///////////////////////////////////////////////////

void why::World::on_mouse_move(const clan::InputEvent &evt)
{
	if (!GameState::is_active(GameStateValue::Playing))
	{
		return;
	}
	using namespace clan;

	float scale_x = 0.0f, scale_y = 0.0f;
	m_paddle->get_scale(scale_x, scale_y);

	const float paddle_height(m_paddle->get_height() * scale_y);
	const float paddle_width(m_paddle->get_width() * scale_x);
	Pointf pp(m_paddle->get_position());

	const float paddle_middle = paddle_width / 2.0f;

	if (evt.mouse_pos.x - paddle_middle < m_area.left)
	{
		pp.x = m_area.left;
	}
	else if (evt.mouse_pos.x + paddle_middle > m_area.right)
	{
		pp.x = m_area.right - paddle_width;
	}
	else
	{
		pp.x = evt.mouse_pos.x - paddle_middle;
	}

	pp.y = m_area.bottom - (paddle_height + m_area.get_height() * m_paddle_btm_margin);

	m_paddle->set_position(pp);

	if (!m_ball->is_moving())
	{
		const Pointf bp(pp.x + (paddle_middle - m_ball->get_width() / 2.0f), pp.y - paddle_height - 1.0f);
		m_ball->set_position(bp);

	}
}

void why::World::on_mouse_click(const clan::InputEvent &evt)
{
	if (!GameState::is_active(GameStateValue::Playing))
	{
		return;
	}
	assert(m_level && m_ball);

	if (!m_level->is_started() || (m_level->is_started() && m_level->is_paused()))
	{
		assert(!m_ball->is_moving());
		m_level->start();
		m_ball->initial_shoot(clan::Vec2f(0, -5.0f));
		m_paddle->add_modifier(new WidthModifier(*m_paddle, m_pworld.get_pc(), 1.5f));
	}
}

void why::World::on_kbd_up(const clan::InputEvent &evt)
{
	using namespace clan;

	if (m_paddle_angle_mod != 0)
	{
		m_paddle_angle_mod = 0;

		m_paddle->set_rotation(Angle(static_cast<float>(m_paddle_angle_mod), AngleUnit::angle_degrees));
	}
}

void why::World::on_kbd_down(const clan::InputEvent &evt)
{	
	using namespace clan;

	if (evt.str == "a")
	{
		m_paddle_angle_mod = -15;
	}
	else if (evt.str == "d")
	{
		m_paddle_angle_mod = 15;
	}
	m_paddle->set_rotation(Angle(static_cast<float>(m_paddle_angle_mod), AngleUnit::angle_degrees));
}