#pragma once

namespace why
{
	class EffectBase
	{
	public:
		EffectBase(unsigned int duration_ms, unsigned int shots = 1, clan::Rectf area = clan::Rectf());
		virtual ~EffectBase();

		virtual void update(clan::ubyte64 time_elapsed_ms) = 0;
		virtual void draw(clan::Canvas &c) = 0;

		virtual bool is_ready() const;
	protected:
		bool m_is_ready;
		unsigned int m_shots, m_duration_ms;
		clan::Rectf m_area;
	};

	class DeathFlashEffect : public EffectBase
	{
	public:
		DeathFlashEffect(unsigned int duration_ms, unsigned int shots = 1, clan::Rectf area = clan::Rectf());
		virtual ~DeathFlashEffect();

		virtual void update(clan::ubyte64 time_elapsed_ms);
		virtual void draw(clan::Canvas &c);
	private:
		void timer_callback();

		bool m_timer_started;
		clan::Timer m_timer;
		unsigned int m_timer_interval_ms, m_callback_call_times;

		float m_start_alpha;
		clan::Colorf m_flash_color;
	};
}