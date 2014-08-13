#include "pch.hpp"
#include "why_game_object_modifier.hpp"

//////////////////////////////////////////////////////////
// GameObjectModifierBase
//////////////////////////////////////////////////////////

why::GameObjectModifierBase::GameObjectModifierBase(GameObjectBase *obj, 
	clan::PhysicsContext *pc, long mod_duration_ms) : m_obj(obj), 
	m_mod_duration_ms(mod_duration_ms), m_pc(pc), m_time_elapsed_on_apply(0)
{

}

why::GameObjectModifierBase::~GameObjectModifierBase()
{

}

why::GameObjectModifierBase::GameObjectModifierBase(const GameObjectModifierBase &cpy)
{
	m_pc = cpy.m_pc;
	m_mod_duration_ms = cpy.m_mod_duration_ms;
	m_obj = cpy.m_obj;
	m_time_elapsed_on_apply = cpy.m_time_elapsed_on_apply;
}

const why::GameObjectBase *why::GameObjectModifierBase::get_object() const
{
	return m_obj;
}

void why::GameObjectModifierBase::set_object(GameObjectBase *obj)
{
	assert(obj);
	m_obj = obj;
}

void why::GameObjectModifierBase::update(clan::ubyte64 time_elapsed_ms)
{
	// by default do nothiing
}

//////////////////////////////////////////////////////////
// WidthModifier
//////////////////////////////////////////////////////////

why::WidthModifier::WidthModifier(CollidableObject *obj, clan::PhysicsContext *pc, float scale_x,
	long mod_duration_ms) : GameObjectModifierBase(obj, pc, mod_duration_ms),
	m_scale_x(scale_x), m_is_reset(false)
{
}

why::WidthModifier::~WidthModifier()
{
	reset();
}

why::WidthModifier::WidthModifier(const WidthModifier &cpy) : GameObjectModifierBase(cpy)
{
	m_is_reset = cpy.m_is_reset;
	m_scale_x = cpy.m_scale_x;
}

void why::WidthModifier::update(clan::ubyte64 time_elapsed_ms)
{
	if ((clan::System::get_time() - m_time_elapsed_on_apply) >= m_mod_duration_ms)
	{
		reset();
	}
}

void why::WidthModifier::apply()
{
	using namespace clan;

	try
	{
		CollidableObject &o = dynamic_cast <CollidableObject &>(*m_obj);
		o.set_scale(m_scale_x, 1.0f);

		clan::PolygonShape *ps = dynamic_cast<clan::PolygonShape *>(m_obj);
		if (ps)
		{
			ps->set_as_box((m_obj->get_width() * m_scale_x) / 2.0f, m_obj->get_height() / 2.0f);
		}

		FixtureDescription fd = o.fixture_description(*m_pc);
		fd.set_shape(dynamic_cast<clan::Shape &>(o));
		BodyDescription bd = o.body_description(*m_pc);
		const Vec2f pos(o.get_position());

		Body b(*m_pc, bd);
		b.set_data(&o);
		Fixture f(*m_pc, b, fd);

		o.body().kill();
		o.fixture().kill();

		o.body() = b;
		o.fixture() = f;

		o.set_position(pos);

		m_time_elapsed_on_apply = clan::System::get_time();
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

		CollidableObject &o = dynamic_cast <CollidableObject &>(*m_obj);
		o.set_scale(1.0f, 1.0f);

		clan::PolygonShape *ps = dynamic_cast<clan::PolygonShape *>(m_obj);
		if (ps)
		{
			ps->set_as_box(m_obj->get_width() / 2.0f, m_obj->get_height() / 2.0f);
		}

		FixtureDescription fd = o.fixture_description(*m_pc);
		fd.set_shape(dynamic_cast<clan::Shape &>(o));
		BodyDescription bd = o.body_description(*m_pc);

		Body b(*m_pc, bd);
		b.set_data(&o);
		Fixture f(*m_pc, b, fd);

		const Vec2f pos(o.get_position());

		o.body().kill();
		o.fixture().kill();

		o.body() = b;
		o.fixture() = f;

		o.set_position(pos);

		m_is_reset = true;
	}
	catch (std::exception &e)
	{
		WHY_LOG() << e.what();
	}
}
