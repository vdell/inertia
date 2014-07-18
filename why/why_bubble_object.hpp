#pragma once

#include "why_game_object.hpp"

namespace why
{
	class BubbleObject : public MovingObject, public clan::CircleShape
	{
	public:
		BubbleObject(long id, clan::Canvas *canvas, clan::Sprite sprite, clan::PhysicsContext &pc,
			const SettingsManager &sm, const std::string &name = "bubble_object");

		virtual ~BubbleObject();

		BubbleObject(const BubbleObject &cpy);

		virtual void on_collision_begin(clan::Body &b);
		virtual void on_collision_end(clan::Body &b);
		virtual bool should_collide_with(clan::Body &b);

		void draw(clan::Canvas &c);

		bool has_collided_with_paddle() const;
	private:
		clan::Vec2f m_force;
		boost::timer::cpu_timer m_timer;
		bool m_direction_left;
		bool m_has_collided_with_paddle;
	
	};
}