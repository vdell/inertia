#include "pch.hpp"
#include "why_music_player.hpp"

why::MusicPlayer::MusicPlayer(const SettingsManager &sm) : m_fade_filter(1.0f), m_sm(sm),
m_manually_stopped(false)
{
}

why::MusicPlayer::~MusicPlayer()
{

}

void why::MusicPlayer::set_soundtrack(const Soundtrack &st)
{
	m_sound_output = clan::SoundOutput(44100);
	m_volume = m_sm.get_as_float("game.sound.music_volume", 0.3f);
	m_soundtrack = st;
	m_cur_song_it = m_soundtrack.m_tracks.begin();
}

void why::MusicPlayer::update()
{
	if (!m_session.is_playing() && !m_manually_stopped)
	{
		if (!GameState::is_active(GameStateValue::Menu))
		{
			next();
		}
		play();
	}
}

void why::MusicPlayer::play()
{
	if (m_soundtrack.m_tracks.empty())
	{
		return;
	}
	m_manually_stopped = false;

	if (GameState::is_active(GameStateValue::Menu) && !m_session.is_playing())
	{
		m_session = m_soundtrack.m_menu_music.prepare(true, &m_sound_output);
		m_session.set_volume(m_volume);
		m_session.play();
		return;
	}
	
	if (!m_session.is_playing())
	{
		if (m_cur_song_it == m_soundtrack.m_tracks.end())
		{
			m_cur_song_it = m_soundtrack.m_tracks.begin();
		}
		m_session = m_cur_song_it->prepare(false, &m_sound_output);
		m_session.set_volume(m_volume);
		m_session.play();
	}
}

void why::MusicPlayer::stop()
{
	if (!m_session.is_null() && m_session.is_playing())
	{
		m_session.stop();
		m_manually_stopped = true;
	}
}

void why::MusicPlayer::next()
{
	if (m_soundtrack.m_tracks.empty() || GameState::is_active(GameStateValue::Menu))
	{
		return;
	}
	if (m_cur_song_it == m_soundtrack.m_tracks.end())
	{
		m_cur_song_it = m_soundtrack.m_tracks.begin();
	}
	else
	{
		++m_cur_song_it;
	}
}

void why::MusicPlayer::previous()
{
	if (m_soundtrack.m_tracks.empty() || GameState::is_active(GameStateValue::Menu))
	{
		return;
	}
	if (m_cur_song_it == m_soundtrack.m_tracks.begin())
	{
		m_cur_song_it = m_soundtrack.m_tracks.end();
		--m_cur_song_it;
	}
	else
	{
		--m_cur_song_it;
	}
}

void why::MusicPlayer::shuffle()
{
	using namespace std;

	std::shuffle(m_soundtrack.m_tracks.begin(), m_soundtrack.m_tracks.end(), mt19937(m_rdev()));
}

void why::MusicPlayer::restart_current()
{
	m_session.stop();
	m_session.set_position(0);
	play();
}