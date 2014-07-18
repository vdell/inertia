#include "pch.hpp"
#include "why_bubble_object.hpp"
#include "why_paddle_object.hpp"

why::BubbleObject::BubbleObject(long id, clan::Canvas *canvas, clan::Sprite sprite,
	clan::PhysicsContext &pc, const SettingsManager &sm, const std::string &name) :
	MovingObject(id, canvas, sprite, pc, name), clan::CircleShape(pc), m_has_collided_with_paddle(false)
{
	using namespace clan;

	BodyDescription bd(pc);
	bd.set_type(BodyType::body_dynamic);
	bd.set_gravity_scale(0.0f);
	bd.allow_sleep(false);

	m_body = Body(pc, bd);
	m_body.set_data(this);

	FixtureDescription fd(pc);
	fd.set_density(1.0f);
	fd.set_friction(0.0f);
	fd.set_restitution(1.0f);
	fd.set_shape(*this);

	m_fixture = Fixture(pc, m_body, fd);
	m_body.reset_mass_data();
	set_radius((float(get_width() / 2.0f)));

	m_timer.start();
	m_force = Vec2f(0.0f, 1.0f);
}

why::BubbleObject::~BubbleObject()
{

}

why::BubbleObject::BubbleObject(const BubbleObject &cpy) : MovingObject(cpy), clan::CircleShape(cpy)
{

}

void why::BubbleObject::on_collision_begin(clan::Body &b)
{
	auto paddle = dynamic_cast<const PaddleObject*>(b.get_data());
	if (paddle)
	{
		m_has_collided_with_paddle = true;
		m_sprite = clan::Sprite();
	}
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