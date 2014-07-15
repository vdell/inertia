#pragma once

#include "why_game_object.hpp"
#include "why_gui_object.hpp"

namespace why
{
	class Menu;

	/*! @brief Menu item class
	 *
	 * @detail Contains information about a single menu item. The items are positioned 
	 *         by the parent menu.
	 */
	class MenuItem : public GuiObject
	{
	public:
		/*! @brief Constructor
		 * 
		 * @param parent The parent window
		 * @param f The font in which the item will be drawn
		 * @param id Unique ID for the menu item.
		 * @param text Items' text (i.e. label)
		 **/
		MenuItem(clan::DisplayWindow &parent, clan::Font f, unsigned int id, const std::string &text);
		
		/*! @brief Constructor 
		 *
		 * @detail Creates an invalid menu item (defined by setting the ID to -1).
		  **/
		MenuItem();
		
		//! Destructor
		virtual ~MenuItem();

		//! Draws the menu item with the specified font (given in the ctor)
		void draw(clan::Canvas &c);

		//! Returns the items' text (i.e. label)
		const std::string &get_text() const;

		/* @brief Changes the items text (i.e. label)
		 *
		 * @param text The new text
		 **/
		void set_text(const std::string &text);

		//! Returns true if the item is invalid (id == -1) or false otherwise.
		bool is_invalid() const;
	private:
		std::string m_text;
	};


	typedef std::deque <MenuItem> MenuItemDeque;

	class Menu : public GuiObject
	{
	public:
		Menu(clan::DisplayWindow &parent, clan::Font f, clan::Sprite sprite,
			clan::Pointf pos = clan::Pointf(), int id = -1, const std::string &name = "menu");
		virtual ~Menu();

		bool add_item(const MenuItem &item);
		const MenuItem &get_item(unsigned int id) const;

		const MenuItem &get_selection() const;

		void show(unsigned int id, bool value = true);

		void draw(clan::Canvas &c);

		void on_mouse_move(const clan::InputEvent &e);
		void on_mouse_click(const clan::InputEvent &e);

		Menu &operator << (const MenuItem &item);
	private:
		MenuItem &get_item(unsigned int id);
		void update_positions();
		void reset_positions();

		clan::Rectf get_lowest_item() const;

		static MenuItem m_invalid_menu_item;
		clan::Gradient m_gradient;
		MenuItemDeque m_items;
		int m_selected_item_id;
		clan::Sprite m_sprite;
	};
}