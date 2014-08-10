#pragma once

#include "why_game_object.hpp"

namespace why
{
	/*! @brief Paddle object
	*
	* @detail The paddle is moved by the user and it can only move horizontally.
	*/
	class PaddleObject : public MovingObject, public clan::PolygonShape
	{
	public:
		PaddleObject(long id, clan::Canvas *canvas, clan::Sprite sprite, clan::PhysicsContext &pc,
			const SettingsManager &sm, const std::string &name = "paddle_object");
		virtual ~PaddleObject();

		PaddleObject(const PaddleObject &cpy);

		virtual clan::BodyDescription body_description(clan::PhysicsContext &pc) const;

		virtual void on_collision_begin(clan::Body &b);
		virtual void on_collision_end(clan::Body &b);
		virtual bool should_collide_with(clan::Body &b);

		/*! @brief Overrides CollidableObject::draw
		 *
		 * @bug If the base version is not overridden then the sprites' position is incorrect
		 *      when WidthModifier is applied to the object. Reason is still unclear as this
		 *      version does nothing else than calls the base version.
		 */
		virtual void draw(clan::Canvas &c);
	};
}