#include "pch.hpp"
#include "why_game_object.hpp"
#include "why_real2int.hpp"
#include "why_block_object.hpp"
#include "why_ball_object.hpp"

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

}

why::GameObjectBase::GameObjectBase(const GameObjectBase &cpy)
{
	m_name = cpy.m_name;
	m_id = cpy.m_id;
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
	return;
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

clan::Pointf why::StaticObject::get_sprite_position() const
{
	return clan::Pointf(m_rect.left, m_rect.top);
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
	return m_rect.get_size();
}

clan::Rectf why::StaticObject::get_rect() const
{
	return m_rect;
}

clan::Pointf why::StaticObject::get_position() const
{
	return clan::Pointf(m_rect.left, m_rect.bottom);
}

void why::StaticObject::set_sprite_position(clan::Pointf pos)
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

//////////////////////////////////////////////
// CollidableObject
//////////////////////////////////////////////

why::CollidableObject::CollidableObject(long id, clan::Canvas *canvas, clan::Sprite sprite,
	clan::PhysicsContext &pc, const std::string &name) : StaticObject(id, sprite, name)
{
	using namespace clan;
	BodyDescription bd(pc);
	bd.set_type(BodyType::body_static);
	m_body = Body(pc, bd);
	m_body.set_data(this);
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

void why::CollidableObject::enable_collision(bool value)
{
	m_collisions_enabled = value;
	m_fixture.set_as_sensor(!value);
}

bool why::CollidableObject::is_collision_enabled() const
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
	return is_collision_enabled();
}

clan::Vec2f why::CollidableObject::get_body_position() const
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

void why::CollidableObject::set_body_position(clan::Vec2f pos)
{
	m_body.set_position(pos);
}

void why::CollidableObject::set_rotation(clan::Angle a)
{
	m_body.set_angle(a);
	StaticObject::set_rotation(a);
}

void why::CollidableObject::align_sprite_with_body()
{
	using namespace clan;
	set_sprite_position(Pointf(get_body_position().x - get_width() / 2,
		get_body_position().y - get_height() / 2));
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
	if (is_destruction_enabled())
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
	return m_health == 0;
}

bool why::DestroyableObject::is_destruction_enabled() const
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