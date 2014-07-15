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
		GuiObject(clan::DisplayWindow &parent, clan::Font font,
			clan::Sizef size = clan::Sizef(), clan::Pointf pos = clan::Pointf(), int id = -1, 
			const std::string &name = "gui_object");
		GuiObject();
		virtual ~GuiObject();

		int get_id() const;
		const std::string &get_name() const;

		const clan::Font &get_font() const;

		void set_font(clan::Font f);

		virtual bool operator == (const GuiObject &cmp) const;

		virtual void show(bool value);
		bool is_shown() const;

		void set_position(clan::Pointf pos);
		clan::Pointf get_position() const;

		void set_size(clan::Sizef size);
		clan::Sizef get_size() const;

		void set_parent(clan::DisplayWindow &parent);
		void set_name(const std::string &name);
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