#pragma once

#include "why_game_object.hpp"

namespace why
{
	/*! @brief Ball object
	*
	* @detail Ball objects collide with everything and cause damage to block objects.
	*/
	class BallObject : public MovingObject, public clan::CircleShape
	{
	public:
		BallObject(ResourceId id, clan::Canvas *canvas, clan::Sprite sprite, clan::PhysicsContext &pc,
			unsigned int damage, const SettingsManager &sm, const std::string &name = "ball object");
		virtual ~BallObject();

		BallObject(const BallObject &cpy);

		void initial_shoot(clan::Vec2f impulse_force);

		virtual void on_collision_begin(clan::Body &b);
		virtual void on_collision_end(clan::Body &b);
		virtual bool should_collide_with(clan::Body &b);

		unsigned int get_damage() const;
		void set_damage(unsigned int dmg);

		virtual void update(clan::ubyte64 time_elapsed_ms);
	private:
		unsigned int m_block_col_count;
		unsigned int m_damage;
	};
}