#include "why_game_object.hpp"

namespace why
{
	class GameObjectModifierBase
	{
	public:
		GameObjectModifierBase(GameObjectBase &obj, long mod_duration_ms = -1);

		virtual ~GameObjectModifierBase();

		GameObjectBase &get_object() const;
	
		virtual void apply() = 0;
		virtual void reset() = 0;
		virtual void update(clan::ubyte64 time_elapsed_ms);
	protected:
		long m_mod_duration_ms;
		GameObjectBase &m_obj;
	};

	class WidthModifier : public GameObjectModifierBase
	{
	public:
		WidthModifier(CollidableObject &obj, float scale_x, long mod_duration_ms = -1);
		
		virtual ~WidthModifier();

		virtual void apply();
		virtual void reset();
	private:
		bool m_is_reset;
		float m_scale_x;
	};
}