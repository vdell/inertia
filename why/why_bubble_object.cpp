#include "pch.hpp"
#include "why_bubble_object.hpp"
#include "why_paddle_object.hpp"

why::BubbleObject::BubbleObject(long id, clan::Canvas *canvas, clan::Sprite sprite,
	clan::PhysicsContext &pc, const SettingsManager &sm, const std::string &name) :
	MovingObject(id, canvas, sprite, pc, name), clan::CircleShape(pc), 
	m_has_collided_with_paddle(false)
{
	using namespace clan;

	m_body = Body(pc, body_description(pc));

	// For some reason the body type is set to static (by ClanLib) even though 
	// body_description() returns BodyType::body_dynamic. 
	m_body.set_type(BodyType::body_dynamic);
	m_body.set_gravity_scale(0.0f);
	m_body.set_data(this);

	clan::FixtureDescription fd(fixture_description(pc));
	fd.set_shape(*this);
	m_fixture = Fixture(pc, m_body, fd);
	set_radius(static_cast<float>(get_width() / 2.0f));

	m_timer.start();
	m_force = Vec2f(0.0f, 1.0f);
}

why::BubbleObject::~BubbleObject()
{

}

why::BubbleObject::BubbleObject(const BubbleObject &cpy) : MovingObject(cpy), clan::CircleShape(cpy)
{
	m_has_collided_with_paddle = cpy.m_has_collided_with_paddle;
	m_force = cpy.m_force;
	m_timer = cpy.m_timer;
}

void why::BubbleObject::on_collision_begin(clan::Body &b)
{
	if (dynamic_cast<const PaddleObject*>(b.get_data()))
	{
		set_has_collided_with_paddle(true);
		m_sprite = clan::Sprite();
	}
}

void why::BubbleObject::set_has_collided_with_paddle(bool value)
{
	m_has_collided_with_paddle = value;
}

void why::BubbleObject::on_collision_end(clan::Body &b)
{

}
bool why::BubbleObject::should_collide_with(clan::Body &b)
{
	return true;
}

void why::BubbleObject::draw(clan::Canvas &c)
{
	if (m_timer.elapsed().wall % 2 == 0)
	{
		m_force.x = m_force.y + std::rand() % 3;
	}
	else
	{
		m_force.x = ((m_force.y + std::rand() % 3)*-1);
	}

	m_body.apply_force_to_center(m_force);
	MovingObject::draw(c);
}

bool why::BubbleObject::has_collided_with_paddle() const
{
	return m_has_collided_with_paddle;
}

clan::BodyDescription why::BubbleObject::body_description(clan::PhysicsContext &pc) const
{
	clan::BodyDescription bd(pc);
	bd.set_type(clan::BodyType::body_dynamic);
	bd.set_gravity_scale(0.0f);
	bd.allow_sleep(false);
	return pc;
}