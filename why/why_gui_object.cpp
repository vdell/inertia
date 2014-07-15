#include "pch.hpp"
#include "why_gui_object.hpp"

why::GuiObject::GuiObject(clan::DisplayWindow &parent, clan::Font font,
	clan::Sizef size, clan::Pointf pos, int id, const std::string &name) 
{
	initialize(&parent, font, size, pos, id, name);
}

why::GuiObject::GuiObject()
{
	initialize(nullptr, clan::Font(), clan::Size(), clan::Pointf(), -1, "Invalid GUI object");
}

why::GuiObject::~GuiObject()
{

}

void why::GuiObject::initialize(clan::DisplayWindow *parent, clan::Font f,
	clan::Sizef size, clan::Pointf pos, int id, const std::string &name)
{
	if (parent)
	{
		set_parent(*parent);
	}
	else
	{
		m_parent = nullptr;
	}

	set_id(id);
	set_position(pos);
	set_font(f);
	set_size(size);
	set_name(name);
	show(true);
}

void why::GuiObject::set_parent(clan::DisplayWindow &parent)
{
	m_parent = &parent;
}

void why::GuiObject::show(bool value)
{
	m_shown = value;
}

bool why::GuiObject::is_shown() const
{
	return m_shown;
}

int why::GuiObject::get_id() const
{
	return m_id;
}

void why::GuiObject::set_id(int id)
{
	m_id = id;
}

const std::string &why::GuiObject::get_name() const
{
	return m_name;
}

const clan::Font &why::GuiObject::get_font() const
{
	return m_font;
}

void why::GuiObject::set_font(clan::Font f)
{
	m_font = f;
}

bool why::GuiObject::operator==(const GuiObject &cmp) const
{
	return m_id == cmp.m_id;
}

void why::GuiObject::set_position(clan::Pointf pos)
{
	m_pos = pos;
}

clan::Pointf why::GuiObject::get_position() const
{
	return m_pos;
}

void why::GuiObject::set_size(clan::Sizef size)
{
	m_size = size;
}

clan::Sizef why::GuiObject::get_size() const
{
	return m_size;
}

void why::GuiObject::set_name(const std::string &name)
{
	m_name = name;
}