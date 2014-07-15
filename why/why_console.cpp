#include "pch.hpp"
#include "why_console.hpp"

why::ConsoleWindow::ConsoleWindow(clan::DisplayWindow &parent) : m_parent(&parent),
m_show(false)
{
	m_color = clan::Colorf("#0f1f2b");
	m_color.set_alpha(0.8f);
}

why::ConsoleWindow::~ConsoleWindow()
{

}

void why::ConsoleWindow::draw(clan::Canvas &c)
{
	using namespace clan;
	if (!is_shown()) return;

	c.fill_rect(Rectf(0, 0, Sizef
		(static_cast<float>(c.get_width()), 
		static_cast<float>(c.get_height() / 2.0f))), m_color);
}

bool why::ConsoleWindow::is_shown() const
{
	return m_show;
}

void why::ConsoleWindow::show(bool value)
{
	m_show = value;
}

boost::any why::ConsoleWindow::process_command(const std::string &cmd)
{
	return boost::any();
}

void why::ConsoleWindow::output_line(const std::string &line)
{

}

void why::ConsoleWindow::output(const std::string &data)
{

}