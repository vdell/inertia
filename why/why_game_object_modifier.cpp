#include "pch.hpp"
#include "why_game_object_modifier.hpp"

//////////////////////////////////////////////////////////
// GameObjectModifierBase
//////////////////////////////////////////////////////////

why::GameObjectModifierBase::GameObjectModifierBase(GameObjectBase &obj, 
	long mod_duration_ms) : m_obj(obj), m_mod_duration_ms(mod_duration_ms)
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

why::WidthModifier::WidthModifier(CollidableObject &obj, float scale_x,
	long mod_duration_ms) : GameObjectModifierBase(obj, mod_duration_ms),
	m_scale_x(scale_x), m_orig_body_width(-1.0f)
{

}

why::WidthModifier::~WidthModifier()
{
	reset();
}

void why::WidthModifier::apply()
{
	CollidableObject &o = dynamic_cast <CollidableObject &>(m_obj);
	o.set_scale(m_scale_x, 1.0f);

	clan::PolygonShape *ps = dynamic_cast<clan::PolygonShape *>(&m_obj);
	if (ps)
	{
		ps->set_as_box(m_obj.get_width() * m_scale_x, m_obj.get_height());
		
	}
}

void why::WidthModifier::reset()
{
	CollidableObject &o = dynamic_cast <CollidableObject &>(m_obj);
	o.set_scale(1.0f, 1.0f);

	clan::PolygonShape *ps = dynamic_cast<clan::PolygonShape *>(&m_obj);
	if (ps)
	{
		ps->set_as_box(m_obj.get_width(), m_obj.get_height());
	}
}
