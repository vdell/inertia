#pragma once

#include "why_resource.hpp"
#include "why_settings_manager.hpp"

namespace why
{
	typedef std::deque <clan::SoundBuffer> SoundBufferDeque;

	struct Soundtrack
	{
		std::string m_filename;
		clan::SoundBuffer m_menu_music;
		SoundBufferDeque m_tracks;
	};

	/*! @brief Resource manager class
	 *
	 * @detail This class takes care of loading/unloading of game resources. The loading function
	 *         is called by the why::Application instance and that instance will pass the manager
	           to other classes that need access to resources.
	 **/
	class ResourceManager
	{
	public:
		/*! @brief Constructor
		 *
		 * @param parent The parent window
		 * @param sm  Reference to the settings manager.
		 **/
		ResourceManager(clan::DisplayWindow *parent, const SettingsManager &sm);
		
		//! Destructor
		~ResourceManager();

		/*! @brief Loads resources 
		 *
		 * @param resource_path The root path for resources
		 **/
		void load(const std::string &resouces_path);

		ResourceId get_id(clan::Sprite s) const;

		clan::Sprite get_sprite(ResourceId id) const;

		//! Returns a copy of the preloaded font
		clan::Font get_font() const;

		//! Returns a font instance with the given height (the font is loaded in every call)
		clan::Font get_font(unsigned int height, bool anti_aliaising = true) const;

		//! Returns a reference to the preloaded font.
		clan::Font &get_font() { return m_font; }

		const Soundtrack &get_soundtrack() const;
	private:
		void load_soundtrack(const std::string &resources_path);
		void load_sprites(const std::string &resources_path);

		std::string m_resources_path;
		const SettingsManager &m_sm;
		SpriteMap m_sprites;
		clan::Font m_font;
		clan::DisplayWindow *m_parent;
		Soundtrack m_soundtrack;
		SoundBufferDeque m_tracks;
	};
}