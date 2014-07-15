#pragma once

namespace why
{
	class World;

	typedef std::deque < std::string > ConsoleOutputDeque;

	class ConsoleWindow
	{
	public:
		ConsoleWindow(clan::DisplayWindow &parent);
		virtual ~ConsoleWindow();

		void show(bool value = true);
		bool is_shown() const;

		virtual void draw(clan::Canvas &c);
		virtual boost::any process_command(const std::string &cmd);
	private:
		void output_line(const std::string &line);
		void output(const std::string &data);

		bool m_show;
		clan::DisplayWindow *m_parent;
		ConsoleOutputDeque m_output;
		clan::Colorf m_color;
		friend class World;
	};
}