#pragma once

namespace why
{
	/*! @brief The settings manager
	 *
	 * @detail The settings manager is used to load a JSON-based settings file and to query the
	  *        loaded values. 
	  */
	class SettingsManager
	{
	public:
		//! Constructor
		SettingsManager();

		//! Destructor
		~SettingsManager();

		//! Load the given file
		void load(const std::string &path);

		//! Save settings to the given file
		void save(const std::string &path);

		//! Returns true if loading succeeded.
		bool is_loaded() const;

		float get_as_float(const std::string &prop_path, float default_value = 0.0f) const;
		int get_as_int(const std::string &prop_path, int default_value = 0) const;
		std::string get_as_str(const std::string &prop_path, const std::string &default_value = std::string()) const;
		bool get_as_bool(const std::string &prop_path, bool default_value) const;

		template <typename T>
		T get_as(const std::string &prop_path, T default_value) const;
	private:
		bool m_is_loaded;
		boost::property_tree::ptree m_ptree;
	};
}