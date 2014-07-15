#include "pch.hpp"
#include "why_settings_manager.hpp"

why::SettingsManager::SettingsManager()
{
	m_is_loaded = false;
}

why::SettingsManager::~SettingsManager()
{

}

void why::SettingsManager::load(const std::string &path)
{
	boost::property_tree::read_json(path, m_ptree);
	m_is_loaded = true;
}

void why::SettingsManager::save(const std::string &path)
{
	boost::property_tree::write_json(path, m_ptree);
}

bool why::SettingsManager::is_loaded() const
{
	return m_is_loaded;
}

template <typename T>
T why::SettingsManager::get_as(const std::string &prop_path, T default_value) const
{
	assert(is_loaded());
	try
	{
		return m_ptree.get<T>(prop_path);
	}
	catch (std::exception &e)
	{
		WHY_LOG() << "Settings error: " << e.what() << ". Using default value \""
			<< default_value << "\" for the property \"" << prop_path << "\"";
	}
	return default_value;
}

float why::SettingsManager::get_as_float(const std::string &prop_path, float default_value) const
{
	const std::string val(get_as_str(prop_path));
	if (val.empty())
	{
		return default_value;
	}
	if (val == "false") return 0.0f;
	else if (val == "true") return 1.0f;
	else return boost::lexical_cast<float>(val);
	
	return default_value;
}

int why::SettingsManager::get_as_int(const std::string &prop_path, int default_value) const
{
	const float val(get_as_float(prop_path, static_cast<float>(default_value)));
	return static_cast<int>(val);
}

bool why::SettingsManager::get_as_bool(const std::string &prop_path,bool default_value) const
{
	const float val(get_as_float(prop_path, static_cast<bool>(default_value)));
	return static_cast<bool>(val);
}

std::string why::SettingsManager::get_as_str(const std::string &prop_path, const std::string &default_value) const
{
	return get_as<std::string>(prop_path, default_value);
}