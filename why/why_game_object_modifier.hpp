#include "why_game_object.hpp"

namespace why
{
	class GameObjectModifierBase
	{
	public:
		GameObjectModifierBase(GameObjectBase *obj, clan::PhysicsContext *pc, long mod_duration_ms = -1);

		virtual ~GameObjectModifierBase();

		GameObjectModifierBase(const GameObjectModifierBase &cpy);

		const GameObjectBase *get_object() const;

		void set_object(GameObjectBase *obj);
	
		virtual void apply() = 0;
		virtual void reset() = 0;
		virtual void update(clan::ubyte64 time_elapsed_ms);
	protected:
		clan::ubyte64 m_time_elapsed_on_apply;
		long m_mod_duration_ms;
		GameObjectBase *m_obj;
		clan::PhysicsContext *m_pc;
	};

	class WidthModifier : public GameObjectModifierBase
	{
	public:
		WidthModifier(CollidableObject *obj, clan::PhysicsContext *pc, float scale_x, long mod_duration_ms = -1);
		
		virtual ~WidthModifier();

		WidthModifier(const WidthModifier &cpy);

		virtual void apply();
		virtual void reset();
		virtual void update(clan::ubyte64 time_elapsed_ms);
	private:
		bool m_is_reset;
		float m_scale_x;
	};
}