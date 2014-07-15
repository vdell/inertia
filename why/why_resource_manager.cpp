#include "pch.hpp"
#include "why_resource_manager.hpp"

why::ResourceManager::ResourceManager(clan::DisplayWindow *parent, const SettingsManager &sm) : 
m_parent(parent), m_sm(sm)
{
	assert(m_parent);
}

why::ResourceManager::~ResourceManager()
{
}

void why::ResourceManager::load(const std::string &resources_path)
{
	using namespace clan;
	m_resources_path = resources_path;
	load_sprites(resources_path);

	WHY_LOG() << "Loading font...";

	FontDescription fd;
	fd.set_anti_alias();
	fd.set_charset(FontDescription::charset_ansi);
	fd.set_typeface_name("Kernel Panic NBP");
	fd.set_height(24);
	m_font = Font(Canvas(*m_parent), fd, resources_path + m_sm.get_as_str("game.paths.fonts", "fonts/") + "KERNEL1.ttf");

	WHY_LOG() << "Font loaded";

	load_soundtrack(resources_path);
}

void why::ResourceManager::load_sprites(const std::string &resources_path)
{
	using namespace clan;

	WHY_LOG() << "Loading sprites...";

	if (!m_sprites.empty())
	{
		m_sprites.clear();
	}
	assert(m_parent);

	typedef SpriteType st;
	typedef ResourceId rid;
	typedef SpriteMetaData smd;

	Canvas c = Canvas(*m_parent);

	std::string sprites_path(resources_path + m_sm.get_as_str("game.paths.art", "art/png"));

	std::string path = sprites_path + "player_ball_default.png";
	m_sprites[smd(st::Other, rid::PlayerBall, path)] = Sprite(c, path);

	path = sprites_path + "red_block_default.png";
	m_sprites[smd(st::Block, rid::BlockRed, path)] = Sprite(c, path);

	path = sprites_path + "blue_block_default.png";
	m_sprites[smd(st::Block, rid::BlockBlue, path)] = Sprite(c, path);

	path = sprites_path + "green_block_default.png";
	m_sprites[smd(st::Block, rid::BlockGreen, path)] = Sprite(c, path);

	path = sprites_path + "yellow_block_default.png";
	m_sprites[smd(st::Block, rid::BlockYellow, path)] = Sprite(c, path);

	path = sprites_path + "grey_block_default.png";
	m_sprites[smd(st::Block, rid::BlockGrey, path)] = Sprite(c, path);

	path = sprites_path + "purple_block_default.png";
	m_sprites[smd(st::Block, rid::BlockPurple, path)] = Sprite(c, path);

	path = sprites_path + "brown_block_default.png";
	m_sprites[smd(st::Block, rid::BlockBrown, path)] = Sprite(c, path);

	path = sprites_path + "spacer_block_80x27.png";
	m_sprites[smd(st::Block, rid::BlockTransparent, path)] = Sprite(c, path);

	path = sprites_path + "paddle_default.png";
	m_sprites[smd(st::Other, rid::PlayerPaddle, path)] = Sprite(c, path);

	path = sprites_path + "background.png";
	m_sprites[smd(st::Other, rid::BackgroundGame, path)] = Sprite(c, path);

	path = sprites_path + "background_nogui.png";
	m_sprites[smd(st::Other, rid::BackgroundMenu, path)] = Sprite(c, path);

	path = sprites_path + "main_menu.png";
	m_sprites[smd(st::Other, rid::MainMenuBackground, path)] = Sprite(c, path);

	path = sprites_path + "bubble.png";
	m_sprites[smd(st::Other, rid::Bubble, path)] = Sprite(c, path);

	WHY_LOG() << m_sprites.size() << " sprite(s) loaded";
}

void why::ResourceManager::load_soundtrack(const std::string &resources_path)
{
	using namespace clan;
	using namespace boost::property_tree;
	using namespace std;

	WHY_LOG() << "Loading soundtrack...";

	if (!m_tracks.empty())
	{
		m_tracks.clear();
	}

	string path (resources_path + m_sm.get_as_str("game.paths.music", "audio/music/") + "soundtrack.json");
	ptree strack;
	read_json(path, strack);

	if (strack.empty())
	{
		return;
	}

	m_soundtrack.m_filename = path;
	path = resources_path + m_sm.get_as_str("game.paths.music", "audio/music/");
	m_soundtrack.m_menu_music = SoundBuffer(path + strack.get<string>("soundtrack.menu"));

	for (auto song : strack.get_child("soundtrack.songs"))
	{
		m_soundtrack.m_tracks.push_back(clan::SoundBuffer(path + song.second.data()/*,true*/));
	}

	WHY_LOG() << (m_soundtrack.m_tracks.size() + 1) << " song(s) loaded";
}

clan::Font why::ResourceManager::get_font(unsigned int height, bool anti_aliaising) const
{
	clan::FontDescription fd;
	fd.set_anti_alias(anti_aliaising);
	fd.set_charset(clan::FontDescription::charset_ansi);
	fd.set_typeface_name("Kernel Panic NBP");
	fd.set_height(height);
	return clan::Font((clan::Canvas)(*m_parent), fd, m_resources_path + m_sm.get_as_str("game.paths.fonts", "fonts/") + "KERNEL1.ttf");
}

clan::Font why::ResourceManager::get_font() const
{
	return m_font;
}

clan::Sprite why::ResourceManager::get_sprite( ResourceId id ) const
{
	for (auto &o : m_sprites)
	{
		if (o.first.resource_id() == id)
		{
			return o.second;
		}
	}
	WHY_LOG() << "Resource with the ID " << static_cast<int>(id) << " is not loaded.";
	return clan::Sprite();
}

const why::Soundtrack &why::ResourceManager::get_soundtrack() const
{
	return m_soundtrack;
}