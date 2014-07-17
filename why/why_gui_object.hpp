#pragma once

namespace why
{
	class GuiException : public std::exception
	{
	public:
		GuiException(const std::string &e_msg) : m_emsg(e_msg)
		{

		}
		virtual const char *what() const
		{
			return m_emsg.c_str();
		}
	private:
		std::string m_emsg;
	};


	//! Abstract base class for all GUI items.
	class GuiObject
	{
	public:
		/*! @brief Constructor
		 *
		 * @param parent The parent DisplayWindow
		 * @param font Font for this object
		 * @param size Size of this object
		 * @param pos Position for this object
		 * @param id Unique ID for this object
		 * @param name Name for this object
		 **/
		GuiObject(clan::DisplayWindow &parent, clan::Font font,
			clan::Sizef size = clan::Sizef(), clan::Pointf pos = clan::Pointf(), int id = -1, 
			const std::string &name = "gui_object");
		
		/*! @brief Constructor 
		* 
		* @detail This will create an invalid GUI object.
		**/
		GuiObject();
		
		//! Destructor
		virtual ~GuiObject();

		//! Returns the ID of the object
		int get_id() const;

		//! Returns the name of the object
		const std::string &get_name() const;

		//! Returns the font of the object
		clan::Font get_font();

		//! Sets the font for the object
		void set_font(clan::Font f);

		virtual bool operator == (const GuiObject &cmp) const;

		//! Shows or hides the object.
		virtual void show(bool value);

		//! Returns true if the object is shown, false otherwise
		bool is_shown() const;

		//! Sets the position of the object
		void set_position(clan::Pointf pos);

		//! Returns the position of the object
		clan::Pointf get_position() const;

		//! Sets the size of the object
		void set_size(clan::Sizef size);

		//! Returns the size of the object
		clan::Sizef get_size() const;

		//! Sets the parent of the object
		void set_parent(clan::DisplayWindow &parent);

		//! Sets the name of the object
		void set_name(const std::string &name);
		
		//! Sets the ID of the object
		void set_id(int id);

		virtual void draw(clan::Canvas &c) = 0;
	protected:
		virtual void initialize(clan::DisplayWindow *parent, clan::Font f,
			clan::Sizef size, clan::Pointf pos, int id, const std::string &name);
		
		int m_id;
		std::string m_name;
		bool m_shown;
		clan::DisplayWindow *m_parent;
		clan::Pointf m_pos;
		clan::Sizef m_size;
		clan::Font m_font;
	};
}