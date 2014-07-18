#pragma once

#include "why_block_object.hpp"
#include "why_resource.hpp"
#include "why_resource_manager.hpp"
#include "why_settings_manager.hpp"

namespace why
{
	enum class RowAligment
	{
		Left, Center, Right
	};

	struct Row
	{
	public:
		Row();
		Row(unsigned int idx, RowAligment aligment, bool empty);
		explicit Row(unsigned int idx);

		unsigned int m_index;
		RowAligment m_aligment;
		bool m_empty;
	};

	struct RowCompare
	{
		bool operator() (const Row &lhs, const Row rhs)
		{
			return lhs.m_index < rhs.m_index;
		}
	};

	typedef std::map < Row, GameObjectBasePtrDeque, RowCompare > level_object_row_map;

	//! Forward declaration
	class LevelLoader;

	/*! @brief Level class 
	 *
	 * @detail This class describes a single level in the game world. It takes care of the level 
	 *         objects (adding/removing/drawing/positioning).
	 *         
	 **/
	class Level
	{
	public:
		/*! @brief Default constructor
		 *
		 * @param area The area to which the level is allowed to draw itself.
		 **/
		Level(clan::Rectf area);
		
		//! Destructor
		virtual ~Level();

		/* @brief Loads a level from the given file 
		 *
		 * @param path The path to the level-file
		 * @param c The canvas that the level will be drawn
		 * @param rcm Reference to the resource manager
		 * @param pc Reference to the physics context
		 * @param sm Reference to the settings manager
		 **/
		void load(const std::string &path, clan::Canvas &c, const ResourceManager &rcm,
			clan::PhysicsContext &pc, const SettingsManager &sm );

		/*! @brief Adds an object to the level 
		 *
		 * @param obj The object to be added.
		 *
		 * @detail The object must be dynamically created and the ownership will be taken by the level-instance.
		 **/
		void add_object(Row r, GameObjectBase *obj);

		//! Returns the object with the given name or nullptr if not found.
		const GameObjectBase *get_object(const std::string &name) const;

		//! Removes all added level objects from the level and resets the score.
		void reset();

		/*! @brief Draws the level.
		 * 
		 * @param canvas The canvas to which the objects will be drawn.
		 */
		void draw(clan::Canvas &canvas);

		//! Updates level state by checking for destroyed blocks.
		void update(clan::ubyte64 time_elapsed_ms);

		//! Returns current score
		unsigned int get_score() const;

		/*! @brief Checks if the level is completed
		 *
		 * @return true if there's nothing to destroy (i.e only spacers and undestructable blocks) or false otherwise
		 **/
		bool is_completed() const;
		
		//! Resets score to zero
		void reset_score();

		clan::Rectf get_area() const;

		//! Returns all level objects (= blocks and spacers).
		const level_object_row_map &get_objects() const;

		//! Returns true if the level is started, false otherwise.
		bool is_started() const;

		//! Returns true if the level is paused, false otherwise.
		bool is_paused() const;

		void start();
		void stop();
		void pause();

		/*! @brief Returns the elapsed level time.
		 *
		 * @return A tuple with elapsed hours[0], minutes[1] and seconds[2]
		 **/
		boost::tuple <int,int,int> get_time_elapsed() const;

		/*! @brief Returns the amount of combos that the player has achieved during the level.
		 *
		 * @detail The combo counter is increased every time that the player has more than one block
		 *         falling at the same time.
		 **/
		unsigned int get_combo_count() const;

		//! Resets the combo count to zero.
		void reset_combo_count();
	private:
		float get_row_width(unsigned int idx) const;

		void start_timer();
		void stop_timer();
		void resume_timer();

		bool is_destroyable(const GameObjectBase *gob) const;

		void increase_score();

		static unsigned int m_max_level_time_sec;

		void add_falling_block(const BlockObject *o);
		void remove_falling_block(const BlockObject *o);

		std::deque < const BlockObject * > m_falling_blocks;

		RowAligment m_row_aligment;
		unsigned int m_score, m_combo_count;
		bool m_level_started, m_paused, m_combo_started;
		clan::Rectf m_area;
		level_object_row_map m_objects;
		boost::timer::cpu_timer m_level_timer;
	};

	/* @brief Level loader class 
	 *
	 * @detail This class is used to load level information from the JSON-based level-files. It works by directly
	 *         modifying the given Level-instance
	 **/
	class LevelLoader
	{
	public:
		/*! @brief Constructor 
		 *
		 * @param level_path Path to tje JSON-file
		 * @param to The level being loaded
		 * @param c Canvas for the level objects
		 * @param Referennce to the resource manager
		 * @param pc Reference to the physics context
		 * @param sm Reference to the settings manager
		 **/
		LevelLoader(const std::string &level_path, Level &to, clan::Canvas &c, const ResourceManager &rcm,
			clan::PhysicsContext &pc, const SettingsManager &sm);
		
		//! Destructor
		~LevelLoader();
	private:
		struct block_dummy
		{
			ResourceId rcid;
			std::string name;
			BlockPosition pos;
			const BlockObject *clone_from;
			const BlockObject *pos_parent;
		};

		void load(const std::string &level_path);

		typedef boost::tuple<Row, unsigned int> row_tuple;
		typedef boost::tuple <block_dummy, unsigned int> block_tuple;
		
		row_tuple load_row(const boost::property_tree::ptree &from, unsigned int &row_idx);
		block_tuple load_block(const boost::property_tree::ptree &from);

		typedef std::deque < block_tuple > block_deque;

		void create_empty_rows(Row r, unsigned int count);
		void add_row(Row r, unsigned int repeat_row, const block_deque &blocks);
		void add_block(Row r, unsigned int repeat_block, block_dummy bd);

		std::string get_optional_str(const std::string &name, const boost::property_tree::ptree &from,
			const std::string &default_value = std::string() ) const;

		ResourceId str_to_rc_id(const std::string &s) const;
		RowAligment str_to_row_aligment(const std::string &s) const;

		RowAligment m_global_row_aligment;
		Level &m_to;
		clan::Canvas &m_canvas;
		const ResourceManager &m_rcm;
		const SettingsManager &m_sm;
		clan::PhysicsContext &m_pc;
	};
}