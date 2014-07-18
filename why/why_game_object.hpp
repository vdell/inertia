#pragma once

#include "why_settings_manager.hpp"
#include "why_resource.hpp"

namespace why
{
	//! Forward declaration
	class Level;

	long get_unique_object_id();

	//! An abstract base class for all game objects.
	class GameObjectBase
	{
	public:
		GameObjectBase(long id = -1, const std::string &name = "game_object");
		virtual ~GameObjectBase();

		GameObjectBase(const GameObjectBase &cpy);

		virtual float get_width() const = 0;
		virtual float get_height() const = 0;

		virtual const std::string &get_name() const;
		virtual void set_name(const std::string &name);

		long get_id() const;

		virtual void draw(clan::Canvas &c) = 0;
		virtual void update(clan::ubyte64 time_elapsed_ms);
	protected:
		std::string m_name;
		long m_id;
	};

	class StaticObject : public GameObjectBase
	{
	public:
		/*! @brief Constructor
		*
		* @param sprite The sprite for this object.
		* @param name Name of the object (e.g. "player ball"). Can be empty.
		**/
		StaticObject(long id = -1, clan::Sprite sprite = clan::Sprite(), const std::string &name = "static_object");

		//! Destructor
		virtual ~StaticObject();

		StaticObject(const StaticObject &cpy);

		void draw(clan::Canvas &canvas);		

		float get_width() const;
		float get_height() const;

		clan::Sizef get_size() const;
		clan::Pointf get_position() const;
		clan::Rectf get_rect() const;

		void set_rotation(clan::Angle a);
		clan::Angle get_rotation() const;

		clan::Sprite get_sprite() const;

		clan::Pointf get_sprite_position() const;
	
		void set_sprite_position(clan::Pointf pos);
	protected:
		clan::Sprite m_sprite;
		clan::Rectf m_rect;

		friend class Level;
	};

	typedef std::deque < GameObjectBase * > GameObjectBasePtrDeque;

	class CollidableObject : public StaticObject, public clan::PhysicsObject
	{
	public:
		CollidableObject(long id, clan::Canvas *canvas, clan::Sprite sprite,
			clan::PhysicsContext &pc, const std::string &name = "collidable_object");
		virtual ~CollidableObject();

		CollidableObject(const CollidableObject &cpy);

		void enable_collision(bool value = true);
		bool is_collision_enabled() const;

		virtual void on_collision_begin(clan::Body &b);
		virtual void on_collision_end(clan::Body &b);
		virtual bool should_collide_with(clan::Body &b);

		virtual void set_body_position(clan::Vec2f pos);
		clan::Vec2f get_body_position() const;

		void set_rotation(clan::Angle a);

		clan::Body &body();
		const clan::Body &body() const;

		void align_sprite_with_body();
	protected:
		bool m_collisions_enabled;
		clan::Canvas *m_canvas;
		clan::Fixture m_fixture;
		clan::Body m_body;
	};

	class DestroyableObject : public CollidableObject
	{
	public:
		DestroyableObject(long id, clan::Canvas *canvas, clan::Sprite sprite, 
			unsigned int health, clan::PhysicsContext &pc, 
			const std::string &name = "destroyable_object");
		virtual ~DestroyableObject();

		DestroyableObject(const DestroyableObject &cpy);

		virtual unsigned int hit(unsigned int damage);
		bool is_dead() const;

		void enable_destruction(bool value = true);
		bool is_destruction_enabled() const;
	private:
		bool m_destruction_enabled;
		unsigned int m_health;
	};

	class MovingObject : public CollidableObject
	{
	public:
		MovingObject(long id, clan::Canvas *canvas, clan::Sprite sprite,
			clan::PhysicsContext &pc, const std::string &name = "moving_object");
		virtual ~MovingObject();

		MovingObject(const MovingObject &cpy);

		void stop_movement();
		bool is_moving() const;

		clan::Vec2f get_prev_position() const;
		void set_prev_position(clan::Vec2f pos);

		clan::Vec2f get_smoothed_position() const;
		void set_smoothed_position(clan::Vec2f pos);

		float get_prev_angle() const;
		void set_prev_angle(float angle);

		float get_smoothed_angle() const;
		void set_smoothed_angle(float angle);

		void draw(clan::Canvas &c);
	private:
		clan::Vec2f m_prev_pos, m_smoothed_pos;
		float m_prev_angle, m_smoothed_angle;
	};
}