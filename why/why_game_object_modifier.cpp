#include "pch.hpp"
#include "why_game_object_modifier.hpp"

//////////////////////////////////////////////////////////
// GameObjectModifierBase
//////////////////////////////////////////////////////////

why::GameObjectModifierBase::GameObjectModifierBase(GameObjectBase &obj, 
	clan::PhysicsContext &pc, long mod_duration_ms) : m_obj(obj), 
	m_mod_duration_ms(mod_duration_ms), m_pc(pc)
{

}

why::GameObjectModifierBase::~GameObjectModifierBase()
{

}

why::GameObjectBase &why::GameObjectModifierBase::get_object() const
{
	return m_obj;
}

void why::GameObjectModifierBase::update(clan::ubyte64 time_elapsed_ms)
{
	// by default do nothiing
}

//////////////////////////////////////////////////////////
// WidthModifier
//////////////////////////////////////////////////////////

why::WidthModifier::WidthModifier(CollidableObject &obj, clan::PhysicsContext &pc, float scale_x,
	long mod_duration_ms) : GameObjectModifierBase(obj, pc, mod_duration_ms),
	m_scale_x(scale_x), m_is_reset(false)
{
}

why::WidthModifier::~WidthModifier()
{
	reset();
}

void why::WidthModifier::apply()
{
	using namespace clan;

	try
	{
		CollidableObject &o = dynamic_cast <CollidableObject &>(m_obj);
		o.set_scale(m_scale_x, 1.0f);

		clan::PolygonShape *ps = dynamic_cast<clan::PolygonShape *>(&m_obj);
		if (ps)
		{
			ps->set_as_box((m_obj.get_width() * m_scale_x) / 2.0f, m_obj.get_height() / 2.0f);
		}

		FixtureDescription fd = o.fixture_description(m_pc);
		fd.set_shape(dynamic_cast<clan::Shape &>(o));
		BodyDescription bd = o.body_description(m_pc);

		Body b(m_pc, bd);
		b.set_data(&o);
		Fixture f(m_pc, b, fd);

		o.body().kill();
		o.fixture().kill();

		o.body() = b;
		o.fixture() = f;
	}
	catch (std::exception &e)
	{
		WHY_LOG() << e.what();
	}
}

void why::WidthModifier::reset()
{
	using namespace clan;

	try
	{
		if (m_is_reset) return;

		CollidableObject &o = dynamic_cast <CollidableObject &>(m_obj);
		o.set_scale(1.0f, 1.0f);

		clan::PolygonShape *ps = dynamic_cast<clan::PolygonShape *>(&m_obj);
		if (ps)
		{
			ps->set_as_box(m_obj.get_width() / 2.0f, m_obj.get_height() / 2.0f);
		}

		FixtureDescription fd = o.fixture_description(m_pc);
		fd.set_shape(dynamic_cast<clan::Shape &>(o));
		BodyDescription bd = o.body_description(m_pc);

		Body b(m_pc, bd);
		b.set_data(&o);
		Fixture f(m_pc, b, fd);

		o.body().kill();
		o.fixture().kill();

		o.body() = b;
		o.fixture() = f;

		m_is_reset = true;
	}
	catch (std::exception &e)
	{
		WHY_LOG() << e.what();
	}
}
