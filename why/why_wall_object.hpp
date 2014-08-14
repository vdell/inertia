#pragma once

#include "why_game_object.hpp"

namespace why
{
	enum class WallPosition
	{
		Left, Right, Top, Bottom
	};

	class WallObject : public CollidableObject, public clan::PolygonShape
	{
	public:
		WallObject(WallPosition pos, clan::Rectf area, clan::PhysicsContext &pc, const std::string &name = "wall_object");

		virtual ~WallObject();

		virtual clan::BodyDescription body_description(clan::PhysicsContext &pc) const;
	private:
		static float m_thickness;
		WallPosition m_pos;
		clan::Rectf m_area;
	};
}