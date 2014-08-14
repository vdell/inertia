#include "pch.hpp"
#include "why_wall_object.hpp"

float why::WallObject::m_thickness = 5.0f;

why::WallObject::WallObject(WallPosition pos, clan::Rectf area,
	clan::PhysicsContext &pc, const std::string &name) : CollidableObject(-1, nullptr, clan::Sprite(), pc, name),
	PolygonShape(pc), m_pos(pos), m_area(area)
{
	if (m_pos == WallPosition::Bottom || m_pos == WallPosition::Top)
	{
		set_as_box(m_area.get_width() / 2.0f, m_thickness);
	}
	else
	{
		set_as_box(m_thickness, m_area.get_height() / 2.0f);
	}

	m_body = clan::Body(pc, body_description(pc));
	m_body.set_data(this);

	clan::FixtureDescription fd(fixture_description(pc));
	fd.set_shape(*this);
	m_fixture = clan::Fixture(pc, m_body, fd);
}

why::WallObject::~WallObject()
{

}

clan::BodyDescription why::WallObject::body_description(clan::PhysicsContext &pc) const
{
	using namespace clan;

	BodyDescription bd(pc);

	if (m_pos == WallPosition::Top)
	{
		bd.set_position(Pointf(m_area.left + m_area.get_width() / 2.0f, m_area.top - m_thickness / 2.0f));
	}
	else if (m_pos == WallPosition::Bottom)
	{
		// Not meaningful ATM
	}
	else if (m_pos == WallPosition::Left)
	{
		bd.set_position(Pointf(m_area.left - m_thickness / 2.0f, m_area.top + m_area.get_height() / 2.0f));
	}
	else if (m_pos == WallPosition::Right)
	{
		bd.set_position(Pointf(m_area.right - m_thickness / 2, m_area.top + m_area.get_height() / 2.0f));
	}
	bd.set_type(BodyType::body_static);

	return bd;
}