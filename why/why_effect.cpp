#include "pch.hpp"
#include "why_effect.hpp"

why::EffectBase::EffectBase(unsigned int duration_ms, unsigned int shots,
	clan::Rectf area) : m_duration_ms(duration_ms), m_shots(shots), m_area(area),
	m_is_ready(false)
{

}

why::EffectBase::~EffectBase()
{

}

bool why::EffectBase::is_ready() const
{
	return m_is_ready;
}

/////////////////////////////////////////
// DeathFlashEffect
/////////////////////////////////////////

why::DeathFlashEffect::DeathFlashEffect(unsigned int duration_ms, unsigned int shots,
	clan::Rectf area) : EffectBase(duration_ms, shots, area), m_start_alpha(0.5f),
	m_timer_started(false), m_timer_interval_ms(m_duration_ms / 100), m_callback_call_times(0)
{
	m_flash_color = clan::Colorf("#ed1c24");

	m_flash_color.set_alpha(m_start_alpha);
}

why::DeathFlashEffect::~DeathFlashEffect()
{

}

void why::DeathFlashEffect::update(clan::ubyte64 time_elapsed_ms)
{
	if (!m_timer_started)
	{
		m_timer.func_expired().set(this, &DeathFlashEffect::timer_callback);
		m_timer.start(m_timer_interval_ms, true);
		m_timer_started = true;
	}
}

void why::DeathFlashEffect::draw(clan::Canvas &c)
{
	if (!m_timer_started) return;
	c.fill_rect(m_area, m_flash_color);
}

void why::DeathFlashEffect::timer_callback()
{
	++m_callback_call_times;
	m_flash_color.set_alpha(m_flash_color.get_alpha() - m_flash_color.get_alpha() * 0.05f);

	if (m_callback_call_times * m_timer_interval_ms >= m_duration_ms)
	{
		m_flash_color.set_alpha(0.0f);
		
		m_timer.stop();

		m_is_ready = true;

		// Don't set m_timer_started to false so that the flash color will be completely transparent in the next draw()
	}
}