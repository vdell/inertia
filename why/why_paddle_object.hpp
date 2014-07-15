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
		PaddleObject(ResourceId id, clan::Canvas *canvas, clan::Sprite sprite, clan::PhysicsContext &pc,
			const SettingsManager &sm, const std::string &name = "paddle object");
		virtual ~PaddleObject();

		PaddleObject(const PaddleObject &cpy);

		virtual void update(clan::ubyte64 time_elapsed_ms);

		virtual void on_collision_begin(clan::Body &b);
		virtual void on_collision_end(clan::Body &b);
		virtual bool should_collide_with(clan::Body &b);
	};
}