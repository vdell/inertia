#include "pch.hpp"
#include "why_ball_object.hpp"
#include "why_block_object.hpp"

//////////////////////////////////////////////
// BallObject
//////////////////////////////////////////////

why::BallObject::BallObject(long id, clan::Canvas *canvas, clan::Sprite sprite,
	clan::PhysicsContext &pc, unsigned int damage, const SettingsManager &sm,
	const std::string &name) : MovingObject(id, canvas, sprite, pc, name),
	clan::CircleShape(pc)
{
	m_body = clan::Body(pc, body_description(pc));
	m_body.set_data(this);

	clan::FixtureDescription fd(fixture_description(pc));
	fd.set_shape(*this);
	m_fixture = clan::Fixture(pc, m_body, fd);
	
	set_radius(static_cast<float>(get_width() / 2.0f));
	set_damage(damage);

	m_block_col_count = 0;
}

why::BallObject::~BallObject()
{

}

why::BallObject::BallObject(const BallObject &cpy) : MovingObject(cpy), clan::CircleShape(cpy)
{
	m_damage = cpy.m_damage;
	m_block_col_count = cpy.m_block_col_count;
}

void why::BallObject::on_collision_begin(clan::Body &b)
{
	if (dynamic_cast<BlockObject*>(b.get_data()))
	{
		const clan::Vec2f lv(m_body.get_linear_velocity());

		m_body.set_linear_velocity(clan::Vec2f(lv.x * 1.02f, lv.y * 1.02f));
	}
}

void why::BallObject::on_collision_end(clan::Body &b)
{

}
bool why::BallObject::should_collide_with(clan::Body &b)
{
	return true;
}

unsigned int why::BallObject::get_damage() const
{
	return m_damage;
}

void why::BallObject::set_damage(unsigned int dmg)
{
	m_damage = dmg;
}

void why::BallObject::initial_shoot(clan::Vec2f impulse_force)
{
	m_body.apply_linear_impulse(impulse_force, m_body.get_world_center());
}

clan::BodyDescription why::BallObject::body_description(clan::PhysicsContext &pc) const
{
	clan::BodyDescription bd(pc);
	bd.set_type(clan::BodyType::body_dynamic);
	bd.set_gravity_scale(0.0f);
	bd.allow_sleep(false);
	bd.set_as_bullet(true);
	return bd;
}