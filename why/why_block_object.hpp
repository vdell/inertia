#pragma once

#include "why_game_object.hpp"

namespace why
{
	enum class BlockPosition
	{
		Auto, AfterObject, BeforeObject
	};

	class BlockObjectBase
	{
	public:
		BlockObjectBase();
		virtual ~BlockObjectBase();

		BlockObjectBase(const BlockObjectBase &cpy);

		/* @brief Returns the right margin
		*
		* The base class will return the default margin.
		*/
		virtual float margin_right() const;

		/* @brief Returns the top margin
		*
		* The base class will return the default margin.
		*/
		virtual float margin_top() const;

		/* @brief Returns the left margin
		*
		* The base class will return the default margin.
		*/
		virtual float margin_left() const;

		/* @brief Returns the bottom margin
		*
		* The base class will return the default margin.
		*/
		virtual float margin_bottom() const;

		bool is_positioned();

		void set_destryable(bool value = true);
	protected:
		bool m_is_positioned;
		static const float m_default_margin_x;
		static const float m_default_margin_y;

		friend class Level;
	};

	class SpacerObject : public GameObjectBase, public BlockObjectBase
	{
	public:
		SpacerObject(clan::Size s, const std::string &name = "spacer object");
		virtual ~SpacerObject();

		SpacerObject(const SpacerObject &cpy);

		float get_width() const;
		float get_height() const;

		void draw(clan::Canvas &c);
	private:
		clan::Size m_size;

	};

	/*! @brief Block object
	*
	* @detail Blocks are the objects that the player tries to destroy with his mighty err...balls.
	*/
	class BlockObject : public BlockObjectBase, public DestroyableObject, public clan::PolygonShape
	{
	public:
		BlockObject(long id, clan::Canvas *canvas, clan::Sprite sprite, unsigned int health,
			clan::PhysicsContext &pc, const SettingsManager &sm, const std::string &name = "block object",
			BlockPosition pos = BlockPosition::Auto, const BlockObject *pos_parent = nullptr);
		virtual ~BlockObject();

		BlockObject(const BlockObject &cpy);

		virtual void on_collision_begin(clan::Body &b);
		virtual void on_collision_end(clan::Body &b);
		virtual bool should_collide_with(clan::Body &b);

		virtual unsigned int hit(unsigned int damage, const MovingObject *hitter);
	private:
		BlockPosition m_pos;
		const BlockObject *m_pos_parent_item;

		friend class Level;
	};
}