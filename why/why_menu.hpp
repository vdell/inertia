#pragma once

#include "why_game_object.hpp"
#include "why_gui_object.hpp"

namespace why
{
	//! Forward declaration
	class Menu;

	/*! @brief Menu item class
	 *
	 * @detail Contains information about a single menu item. The items are 
	 *         positioned by the parent menu.
	 */
	class MenuItem : public GuiObject
	{
	public:
		/*! @brief Constructor
		 * 
		 * @param parent The parent menu
		 * @param f The font in which the item will be drawn
		 * @param id Unique ID for the menu item.
		 * @param text Items' text (i.e. label)
		 **/
		MenuItem(Menu &parent, clan::Font f, unsigned int id, const std::string &text);
		
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
	private:
		std::string m_text;
	};

	typedef std::deque <MenuItem> MenuItemDeque;

	/*! @brief A class for showing a menu
	 *
	 * @detail The class is currently used for showing the main menu and it doesn't support
	 *         any fancy features like popup menus etc.
	 **/
	class Menu : public GuiObject
	{
	public:
		/*! @brief Constructor
		 *
		 * @param parent The parent window
		 * @param f Font for drawing menu items
		 * @param sprite Menu sprite
		 * @param pos Menu position
		 * @param id Menu ID
		 * @param name Object name (can be empty)
		 **/
		Menu(clan::DisplayWindow &parent, clan::Font f, clan::Sprite sprite,
			clan::Pointf pos = clan::Pointf(), int id = -1, const std::string &name = "menu");

		//! Destructor
		virtual ~Menu();

		/*! @brief Adds a menu item 
		 *
		 * @param item The item to be added
		 * @return true if the item was added or false otherwise (ID not unique). 
		 **/
		bool add_item(const MenuItem &item);

		//! Returns menu item with the given ID or an invalid item if not found.
		const MenuItem &get_item(unsigned int id) const;

		//! Returns the selected menu item or an invalid item if no selection is active.
		const MenuItem &get_selection() const;

		//! Shows or hides the menu item with the given ID
		void show(unsigned int id, bool value = true);

		void draw(clan::Canvas &c);

		void on_mouse_move(const clan::InputEvent &e);
		void on_mouse_click(const clan::InputEvent &e);

		//! @see Menu::add_item
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