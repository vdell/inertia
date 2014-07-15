#pragma once

#include "why_resource_manager.hpp"
#include "why_settings_manager.hpp"
#include "why_game_state.hpp"

namespace why
{
	class MusicPlayer
	{
	public:
		MusicPlayer(const SettingsManager &sm);
		~MusicPlayer();
		void play();
		void stop();
		void next();
		void previous();

		void restart_current();

		void shuffle();

		void update();

		void set_soundtrack(const Soundtrack &st);
	private:
		bool m_manually_stopped;
		Soundtrack m_soundtrack;
		clan::SoundBuffer_Session m_session;
		clan::FadeFilter m_fade_filter;
		clan::SoundOutput m_sound_output;
		SoundBufferDeque::iterator m_cur_song_it;
		const SettingsManager &m_sm;
		std::random_device m_rdev;
		float m_volume;
	};
}