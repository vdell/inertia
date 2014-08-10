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

		virtual clan::BodyDescription body_description(clan::PhysicsContext &pc) const;

		bool has_collided_with_paddle() const;
		void set_has_collided_with_paddle(bool value = true);
	private:
		clan::Vec2f m_force;
		boost::timer::cpu_timer m_timer;
		bool m_has_collided_with_paddle;
	
	};
}