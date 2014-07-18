#include "pch.hpp"
#include "why_paddle_object.hpp"
#include "why_ball_object.hpp"
#include "why_bubble_object.hpp"
#include "why_purgatory.hpp"

//////////////////////////////////////////////
// PaddleObject
//////////////////////////////////////////////

why::PaddleObject::PaddleObject(long id, clan::Canvas *canvas, clan::Sprite sprite,
	clan::PhysicsContext &pc, const SettingsManager &sm, const std::string &name) :
	MovingObject(id, canvas, sprite, pc, name), clan::PolygonShape(pc)
{
	using namespace clan;
	set_as_box(get_width() / 2.0f, get_height() / 2.0f);

	BodyDescription bd(pc);
	bd.set_type(BodyType::body_kinematic);
	bd.set_gravity_scale(0.0f);

	m_body = clan::Body(pc, bd);
	m_body.set_data(this);

	FixtureDescription fd(pc);

	fd.set_density(sm.get_as_float("game.physics.paddle_density", 1.0f));
	fd.set_friction(sm.get_as_float("game.physics.paddle_friction", 0.0f));
	fd.set_restitution(sm.get_as_float("game.physics.paddle_restitution", 1.0f));

	fd.set_shape(*this);
	m_fixture = Fixture(pc, m_body, fd);
}

why::PaddleObject::~PaddleObject()
{
}

why::PaddleObject::PaddleObject(const PaddleObject &cpy) : MovingObject(cpy), clan::PolygonShape(cpy)
{

}

void why::PaddleObject::on_collision_begin(clan::Body &b)
{

}
void why::PaddleObject::on_collision_end(clan::Body &b)
{

}
bool why::PaddleObject::should_collide_with(clan::Body &b)
{
	return true;
}

void why::PaddleObject::update(clan::ubyte64 time_elapsed_ms)
{

}

