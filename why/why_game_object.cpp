#include "pch.hpp"
#include "why_game_object.hpp"
#include "why_real2int.hpp"
#include "why_game_object_modifier.hpp"

//////////////////////////////////////////////
// Free Functions
//////////////////////////////////////////////

long why::get_unique_object_id()
{
	static int counter = 0;
	return ++counter;
}

//////////////////////////////////////////////
// GameObjectBase
//////////////////////////////////////////////

why::GameObjectBase::GameObjectBase(long id, const std::string &name) : m_name(name), m_id(id)
{
	if (m_id == -1)
	{
		m_id = get_unique_object_id();
	}
}

why::GameObjectBase::~GameObjectBase()
{
	delete_modifiers();
}

why::GameObjectBase::GameObjectBase(const GameObjectBase &cpy)
{
	m_name = cpy.m_name;
	m_id = cpy.m_id;

	m_modifiers = cpy.m_modifiers;

	// TODO: Should clone the modifier.

	for (auto mod : m_modifiers)
	{
		mod->set_object(this);
		mod->apply();
	}
}

void why::GameObjectBase::add_modifier(GameObjectModifierBase *mod)
{
	m_modifiers.push_back(mod);
	m_modifiers.back()->apply();
}

void why::GameObjectBase::delete_modifiers()
{
	for (auto &mod : m_modifiers)
	{
		mod->reset();
		delete mod;
	}
	m_modifiers.clear();
}

const std::string &why::GameObjectBase::get_name() const
{
	return m_name;
}

long why::GameObjectBase::get_id() const
{
	return m_id;
}

void why::GameObjectBase::set_name(const std::string &name)
{
	m_name = name;
}

void why::GameObjectBase::update(clan::ubyte64 time_elapsed_ms)
{
	for (auto mod : m_modifiers)
	{
		mod->update(time_elapsed_ms);
	}
}

bool why::GameObjectBase::is_movable() const
{
	return false;
}

bool why::GameObjectBase::is_collidable() const
{
	return false;
}

bool why::GameObjectBase::is_destroyable() const
{
	return false;
}

//////////////////////////////////////////////
// StaticObject
//////////////////////////////////////////////


why::StaticObject::StaticObject(long id, clan::Sprite sprite, const std::string &name) :
GameObjectBase(id, name), m_sprite(sprite.clone())
{
	if (!m_sprite.is_null())
	{
		m_rect.set_size((clan::Sizef)sprite.get_size());
	}
}

why::StaticObject::~StaticObject()
{
}

why::StaticObject::StaticObject(const StaticObject &cpy) : GameObjectBase(cpy)
{ 
	if (cpy.m_sprite.is_null())
	{
		m_sprite = cpy.m_sprite.clone();
	}
	m_rect = cpy.m_rect;
}

void why::StaticObject::draw(clan::Canvas &canvas)
{
	m_sprite.draw(canvas, m_rect.left, m_rect.top);
}

float why::StaticObject::get_height() const
{
	return get_size().height;
}

float why::StaticObject::get_width() const
{
	return get_size().width;
}

clan::Sizef why::StaticObject::get_size() const
{
	return get_rect().get_size();
}

clan::Rectf why::StaticObject::get_rect() const
{
	return m_rect;
}

clan::Pointf why::StaticObject::get_position() const
{
	return clan::Pointf(get_rect().left, get_rect().bottom);
}

void why::StaticObject::set_position(clan::Pointf pos)
{
	m_rect = clan::Rectf(pos, get_size());
}

void why::StaticObject::set_rotation(clan::Angle a)
{
	if (!m_sprite.is_null())
	{
		m_sprite.set_angle(a);
	}
}

clan::Angle why::StaticObject::get_rotation() const
{
	if (m_sprite.is_null())
	{
		return clan::Angle(0.0f, clan::AngleUnit::angle_degrees);
	}
	return m_sprite.get_angle();
}

clan::Sprite why::StaticObject::get_sprite() const
{
	return m_sprite;
}

void why::StaticObject::set_scale (float x, float y)
{
	m_sprite.set_scale(x, y);
}

void why::StaticObject::get_scale(float &x, float &y) const
{
	m_sprite.get_scale(x, y);
}

//////////////////////////////////////////////
// CollidableObject
//////////////////////////////////////////////

why::CollidableObject::CollidableObject(long id, clan::Canvas *canvas, clan::Sprite sprite,
	clan::PhysicsContext &pc, const std::string &name) : StaticObject(id, sprite, name)
{
	m_collisions_enabled = true;
}

why::CollidableObject::~CollidableObject()
{
	m_body.kill();
}

why::CollidableObject::CollidableObject(const CollidableObject &cpy) : StaticObject(cpy), clan::PhysicsObject(cpy)
{
	m_collisions_enabled = cpy.m_collisions_enabled;
	m_canvas = cpy.m_canvas;
	m_body = cpy.m_body;
	m_fixture = cpy.m_fixture;
}

void why::CollidableObject::draw(clan::Canvas &c)
{
	const clan::Vec2f pos(m_body.get_position());
	float sx = 0.0f, sy = 0.0f;
	m_sprite.get_scale(sx, sy);
	m_sprite.draw(c, pos.x - (get_width() * sx) / 2.0f, pos.y - (get_height() * sy) / 2.0f);
}

void why::CollidableObject::enable_collision(bool value)
{
	m_collisions_enabled = value;
	m_fixture.set_as_sensor(!value);
}

bool why::CollidableObject::is_collidable() const
{
	return m_collisions_enabled;
}

void why::CollidableObject::on_collision_begin(clan::Body &b)
{

}

void why::CollidableObject::on_collision_end(clan::Body &b)
{
}

bool why::CollidableObject::should_collide_with(clan::Body &b)
{
	return is_collidable();
}

clan::Pointf why::CollidableObject::get_position() const
{
	return m_body.get_position();
}

clan::Body &why::CollidableObject::body()
{
	return m_body;
}

const clan::Body &why::CollidableObject::body() const
{
	return m_body;
}

clan::Fixture &why::CollidableObject::fixture()
{
	return m_fixture;
}

const clan::Fixture &why::CollidableObject::fixture() const
{
	return m_fixture;
}

void why::CollidableObject::set_position(clan::Pointf pos)
{
	clan::Pointf pos_body = pos;
	pos_body.x += get_width() / 2;
	pos_body.y += get_height() / 2;
	m_body.set_position(pos_body);
}

void why::CollidableObject::set_rotation(clan::Angle a)
{
	m_body.set_angle(a);
	StaticObject::set_rotation(a);
}

clan::FixtureDescription why::CollidableObject::fixture_description(clan::PhysicsContext &pc) const
{
	clan::FixtureDescription fd(pc);
	fd.set_density(1.0f);
	fd.set_friction(0.0f);
	fd.set_restitution(1.0f);
	return fd;
}

//////////////////////////////////////////////
// DestroyableObject
//////////////////////////////////////////////

why::DestroyableObject::DestroyableObject(long id, clan::Canvas *canvas, clan::Sprite sprite,
	unsigned int health, clan::PhysicsContext &pc, const std::string &name) : CollidableObject
	(id, canvas, sprite, pc, name), m_health(health)
{
	assert(m_health > 0);
	m_destruction_enabled = true;
}

why::DestroyableObject::~DestroyableObject()
{
}

why::DestroyableObject::DestroyableObject(const DestroyableObject &cpy) : CollidableObject(cpy)
{
	m_destruction_enabled = cpy.m_destruction_enabled;
	m_health = cpy.m_health;
}

unsigned int why::DestroyableObject::hit(unsigned int damage)
{
	if (is_destroyable())
	{
		if (damage >= m_health)
		{
			m_health = 0;
		}
		else
		{
			m_health -= damage;
		}
	}
	return m_health;
}

bool why::DestroyableObject::is_dead() const
{
	if (!m_destruction_enabled) return false;
	return m_health == 0;
}

bool why::DestroyableObject::is_destroyable() const
{
	return m_destruction_enabled;
}

void why::DestroyableObject::enable_destruction(bool value)
{
	m_destruction_enabled = value;
}

//////////////////////////////////////////////
// MovingObject
//////////////////////////////////////////////

why::MovingObject::MovingObject(long id, clan::Canvas *canvas, clan::Sprite sprite,
	clan::PhysicsContext &pc, const std::string &name) : CollidableObject(id, canvas, sprite, pc, name)
{

}

why::MovingObject::~MovingObject()
{

}

why::MovingObject::MovingObject(const MovingObject &cpy) : CollidableObject(cpy)
{
	m_smoothed_pos = cpy.m_smoothed_pos;
	m_smoothed_angle = cpy.m_smoothed_angle;
	m_prev_angle = cpy.m_prev_angle;
	m_prev_pos = cpy.m_prev_pos;
}

void why::MovingObject::stop_movement()
{
	using namespace clan;
	m_body.set_linear_velocity(Vec2f(0, 0));
	m_body.set_angular_velocity(Angle(0, AngleUnit::angle_degrees));
}

bool why::MovingObject::is_moving() const
{
	return (m_body.get_linear_velocity() != clan::Vec2f(0, 0));
}

void why::MovingObject::draw(clan::Canvas &c)
{
	if (!m_sprite.is_null())
	{
		m_sprite.draw(c, get_smoothed_position().x - get_width() / 2, get_smoothed_position().y - get_height() / 2);
	}
}

clan::Vec2f why::MovingObject::get_prev_position() const
{
	return m_prev_pos;
}

void why::MovingObject::set_prev_position(clan::Vec2f pos)
{
	m_prev_pos = pos;
}

clan::Vec2f why::MovingObject::get_smoothed_position() const
{
	return m_smoothed_pos;
}

void why::MovingObject::set_smoothed_position(clan::Vec2f pos)
{
	m_smoothed_pos = pos;
}

float why::MovingObject::get_prev_angle() const
{
	return m_prev_angle;
}

void why::MovingObject::set_prev_angle(float angle)
{
	m_prev_angle = angle;
}

float why::MovingObject::get_smoothed_angle() const
{
	return m_smoothed_angle;
}

void why::MovingObject::set_smoothed_angle(float angle)
{
	m_smoothed_angle = angle;
}

bool why::MovingObject::is_movable() const
{
	return true;
}