#pragma once

#include "why_resource_manager.hpp"
#include "why_settings_manager.hpp"
#include "why_game_state.hpp"

namespace why
{
	//! Class for playing music during game play.
	class MusicPlayer
	{
	public:
		/* @brief Constructor 
		 *
		 * @param sm Reference to the settings manager
		 **/
		MusicPlayer(const SettingsManager &sm);
		
		//! Destructor
		~MusicPlayer();

		//! Starts playback
		void play();

		//! Stops playback
		void stop();

		//! Advance to next song
		void next();

		//! Go back to the previous song
		void previous();

		//! Restarts the current song
		void restart_current();

		//! Shuffles the playlist
		void shuffle();

		void update();

		//! Set the soundtrack
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