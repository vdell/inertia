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
	m_scale_x(scale_x)
{

}

why::WidthModifier::~WidthModifier()
{
	reset();
}

void why::WidthModifier::apply()
{
	dynamic_cast<CollidableObject &>(m_obj).set_scale(m_scale_x, 1.0f);
}

void why::WidthModifier::reset()
{
	dynamic_cast<CollidableObject &>(m_obj).set_scale(1.0f, 1.0f);
}
