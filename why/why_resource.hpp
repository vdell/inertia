#pragma once

namespace why
{
	//! Available sprite resources
	enum class ResourceId
	{
		BlockYellow = 1,
		BlockRed,
		BlockBlue,
		BlockGreen,
		BlockGrey,
		BlockPurple,
		BlockTransparent,
		BlockBrown,
		PlayerPaddle = 100,
		Spacer,
		PlayerBall,
		Bubble,
		BackgroundGame,
		BackgroundMenu,
		MainMenuBackground,
		MainMenu
	};

	enum class SpriteType
	{
		Block,
		Other
	};

	class SpriteMetaData
	{
	public:
		SpriteMetaData(SpriteType t, ResourceId id, const std::string &path,
			const std::string &data = std::string() ) : m_type(t), m_id(id), 
			m_path(path), m_data(data) { }
		SpriteType type() const { return m_type; }
		ResourceId resource_id() const { return m_id; }
		const std::string &path() const { return m_path; }
		const std::string &data() const { return m_data; }
	private:
		std::string m_path, m_data;
		SpriteType m_type;
		ResourceId m_id;
	};

	struct SpriteMetaDataCmp
	{
		bool operator() (const  SpriteMetaData &lhs, const SpriteMetaData &rhs) const
		{
			return lhs.resource_id() < rhs.resource_id();
		}
	};

	typedef std::map < SpriteMetaData, clan::Sprite, SpriteMetaDataCmp > SpriteMap;
}