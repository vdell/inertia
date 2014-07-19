#include "pch.hpp"
#include "why_menu.hpp"

////////////////////////////////////////////////////
// MenuItem
////////////////////////////////////////////////////

why::MenuItem::MenuItem(Menu &parent, clan::Font f, unsigned int id, const std::string &text) 
	: GuiObject(parent.get_parent(), f, clan::Sizef(), clan::Pointf(), id, "menu_item"), m_text(text)
{

}

why::MenuItem::MenuItem() : GuiObject()
{
	m_text = "Invalid Menu Item";
}

why::MenuItem::~MenuItem()
{

}

const std::string &why::MenuItem::get_text() const
{
	return m_text;
}

void why::MenuItem::set_text(const std::string &text)
{
	m_text = text;
}

void why::MenuItem::draw(clan::Canvas &c)
{
	if (!is_shown()) return;
	const clan::Sizef ts(m_font.get_text_size(c, m_text));
	const clan::Rectf r = clan::Rectf(m_pos, ts);
	const float x = m_pos.x + m_pos.x / 2;
	const float y = r.top + ts.height * 1.4f;

	m_font.draw_text(c, x, y, m_text);
	// For debugging only
	//c.draw_box(m_rect, clan::Colorf::white);
}

////////////////////////////////////////////////////
// Menu
////////////////////////////////////////////////////

why::MenuItem why::Menu::m_invalid_menu_item = why::MenuItem();

why::Menu::Menu(clan::DisplayWindow &parent, clan::Font f, clan::Sprite sprite,
	clan::Pointf pos, int id, const std::string &name) : GuiObject(parent, f, 
	sprite.get_size(), pos, id, "menu"), m_sprite(sprite)
{
	m_sprite.set_alpha(0.7f);
	
	m_selected_item_id = -1;

	clan::Colorf c1("#0f1f2b");
	c1.set_alpha(0.7f);
	clan::Colorf c2("#142d3d");
	c2.set_alpha(0.7f);
	m_gradient = clan::Gradient(c1, c2);
}

why::Menu::~Menu()
{

}

void why::Menu::show(unsigned int id, bool value)
{
	auto &i = get_item(id);
	assert(!i.is_invalid());
	if (i.is_shown() != value)
	{
		i.show(value);
		update_positions();
	}
}

void why::Menu::reset_positions()
{
	for (auto &i : m_items)
	{
		i.set_position(clan::Pointf());
	}
}

void why::Menu::update_positions()
{
	using namespace clan;
	reset_positions();

	// TOOD: A better way to find out the starting position for the menu items.
	const float min_y = m_pos.y * 1.3f;

	for (auto &i : m_items)
	{
		if (!i.is_shown())
		{
			continue;
		}
		auto lowest_item = get_lowest_item();

		if (lowest_item.top < min_y)
		{
			i.set_position(Pointf(m_pos.x, min_y));
		}
		else
		{
			i.set_position(Pointf(m_pos.x, lowest_item.bottom));
		}
		Font f(get_font());
		const Sizef fsize((Sizef)f.get_text_size(Canvas(*m_parent), i.get_text()));
		i.set_size(Sizef(static_cast<float>(m_sprite.get_width()), fsize.height * 2.0f));
	}
}

clan::Rectf why::Menu::get_lowest_item() const
{
	using namespace clan;
	Rectf ret;
	for (const auto &i : m_items)
	{
		if (i.is_shown())
		{
			const Rectf ir(Rectf(i.get_position(), i.get_size()));
			if (ir.bottom > ret.bottom)
			{
				ret = ir;
			}
		}
	}
	return ret;
}

why::Menu &why::Menu::operator<<(const MenuItem &item)
{
	const bool ok = add_item(item);
	assert(ok);
	return *this;
}

bool why::Menu::add_item(const MenuItem &item)
{
	// Make sure IDs are unique
	if (get_item(item.get_id()).is_valid())
	{
		return false;
	}
	m_items.push_back(item);
	update_positions();
	return true;
}

const why::MenuItem &why::Menu::get_item(unsigned int id) const
{
	for (auto &item : m_items)
	{
		if (item.get_id() == id)
		{
			return item;
		}
	}
	return m_invalid_menu_item;
}

why::MenuItem &why::Menu::get_item(unsigned int id)
{
	for (auto &item : m_items)
	{
		if (item.get_id() == id)
		{
			return item;
		}
	}
	return m_invalid_menu_item;
}

void why::Menu::draw(clan::Canvas &c)
{
	using namespace clan;

	static const int border_width = 4;
	static const int top_bar_height = 54;
	static const int bottom_bar_height = 52;

	Pointf pos = m_pos;
	Sizef size = m_size;
	pos.x += border_width;
	pos.y += top_bar_height;
	size.width -= border_width * 2;
	size.height -= top_bar_height + bottom_bar_height;

	c.fill_rect(Rectf(pos, size), m_gradient);
	m_sprite.draw(c, m_pos.x, m_pos.y);

	for (auto &item : m_items)
	{
		item.draw(c);
	}

	if (m_selected_item_id != -1)
	{
		const auto &item = get_item(m_selected_item_id);
		c.fill_rect(Rectf(item.get_position(), item.get_size()), Colorf(0.0f, 0.0f, 0.0f, 0.2f));
	}
}

void why::Menu::on_mouse_move(const clan::InputEvent &e)
{
	for (const auto &item : m_items)
	{
		if (clan::Rectf(item.get_position(), item.get_size()).contains(e.mouse_pos) && item.is_shown())
		{
			m_selected_item_id = item.get_id();
			return;
		}
	}
	m_selected_item_id = -1;
}

const why::MenuItem &why::Menu::get_selection() const
{
	return get_item(m_selected_item_id);
}