 #include "pch.hpp"
#include "why_level.hpp"
#include "why_purgatory.hpp"

//////////////////////////////////////////////////////////
// Row
//////////////////////////////////////////////////////////

why::Row::Row()
{
}

why::Row::Row(unsigned int idx, RowAligment aligment, bool empty) : 
m_index(idx), m_aligment(aligment), m_empty(empty)
{

}

why::Row::Row(unsigned int idx) : m_index(idx)
{
}

//////////////////////////////////////////////////////////
// LevelLoader
//////////////////////////////////////////////////////////

why::LevelLoader::LevelLoader(const std::string &level_path, Level &to, clan::Canvas &c, 
	const ResourceManager &rcm, clan::PhysicsContext &pc, const SettingsManager &sm) : 
	m_to(to), m_canvas(c), m_rcm(rcm), m_pc(pc), m_sm(sm)
{
	load(level_path);
}

why::LevelLoader::~LevelLoader()
{
}

void why::LevelLoader::load(const std::string &level_path)
{
	using namespace boost;
	using namespace std;

	property_tree::ptree tree;
	property_tree::read_json(level_path, tree);
	assert(!tree.empty());
	unsigned int row_idx = 0;

	m_global_row_aligment = str_to_row_aligment(tree.get<string>("settings.global_row_aligment"));

	// Iterate "row" elements
	for (auto &node : tree.get_child("rows"))
	{
		block_deque blocks;

		auto row_tuple = load_row(node.second, ++row_idx);
		if (row_tuple.get<0>().m_empty)
		{
			create_empty_rows(row_tuple.get <0>(), row_tuple.get<1>());
			continue;
		}
		for (auto &block : node.second)
		{
			if (block.first == "block")
			{
				blocks.push_back(load_block(block.second));
			}
		}
		add_row(row_tuple.get<0>(), row_tuple.get<1>(), blocks);
	}
}

void why::LevelLoader::create_empty_rows(Row r, unsigned int count)
{
	for (unsigned int c = 0; c < count; ++c)
	{
		r.m_index = m_to.get_objects().size();

		// TODO: Empty spacer row height should not be hard coded
		m_to.add_object(r, new SpacerObject(clan::Size((int)m_to.get_area().get_width(), 27)));
	}
}

why::LevelLoader::row_tuple why::LevelLoader::load_row(const boost::property_tree::ptree &from, unsigned int &row_idx)
{
	using namespace boost;
	using namespace std;

	// How many times this row will be repeated
	unsigned int repeat_row = 1;
	const auto repeat_opt = from.get_optional<unsigned int>("repeat");
	if (repeat_opt.is_initialized() && repeat_opt.get() > 0)
	{
		repeat_row = repeat_opt.get();
	}

	// Is this an empty spacer row?
	bool empty = false;
	const auto empty_opt = from.get_optional<unsigned int>("empty");
	if (empty_opt.is_initialized())
	{
		empty = (empty_opt.get() == 1);
	}

	RowAligment ra = m_global_row_aligment;
	// Is the global row alignment overridden?
	const auto ra_opt = from.get_optional<string>("alignment");
	if (ra_opt.is_initialized())
	{
		ra = str_to_row_aligment(ra_opt.get());
	}
	return boost::make_tuple(Row(row_idx, ra, empty), repeat_row);
}

why::LevelLoader::block_tuple why::LevelLoader::load_block(const boost::property_tree::ptree &from)
{
	using namespace boost;

	unsigned int repeatx = 1;
	const auto repeatx_opt = from.get_optional<unsigned int>("repeat-x");
	if (repeatx_opt.is_initialized() && repeatx_opt.get() > 0)
	{
		repeatx = repeatx_opt.get();
	}

	// Should this block be positioned after some other block (name of the block in the value)
	const std::string pafter = get_optional_str("position_after", from); 
	
	// See above
	const std::string pbefore = get_optional_str("position_before", from);
	
	// Default to auto position
	BlockPosition pos = BlockPosition::Auto;

	if (!pafter.empty() && !pbefore.empty())
	{
		WHY_LOG() << "position_after and position_before both defined, ignoring values";
	}
	else if (!pafter.empty())
	{
		pos = BlockPosition::AfterObject;
	}
	else if (!pbefore.empty())
	{
		pos = BlockPosition::BeforeObject;
	}

	const auto rcid = str_to_rc_id(from.get<std::string>("id"));

	const std::string name = get_optional_str("name", from);
	
	// Is this block cloned from another existing block?
	const BlockObject *clone_from = dynamic_cast <const BlockObject *>(m_to.get_object(name));
	if (pos != BlockPosition::Auto)
	{
		auto pos_parent = dynamic_cast<const BlockObject*>(m_to.get_object(pafter.empty() ? pbefore : pafter));
		return make_tuple(block_dummy{ rcid, name, pos, clone_from, pos_parent }, repeatx);
	}
	return make_tuple(block_dummy{ rcid, name, pos, clone_from }, repeatx);
}

std::string why::LevelLoader::get_optional_str(const std::string &name, const boost::property_tree::ptree &from,
	const std::string &default_value) const
{
	const auto val(from.get_optional<std::string>(name));
	if (val.is_initialized())
	{
		return val.get();
	}
	return default_value;
}

why::ResourceId why::LevelLoader::str_to_rc_id(const std::string &str) const
{
	if (str == "green") return ResourceId::BlockGreen;
	if (str == "yellow") return ResourceId::BlockYellow;
	if (str == "blue") return ResourceId::BlockBlue;
	if (str == "red") return ResourceId::BlockRed;
	if (str == "transparent") return ResourceId::BlockTransparent;
	if (str == "grey") return ResourceId::BlockGrey;
	if (str == "purple") return ResourceId::BlockPurple;
	if (str == "brown") return ResourceId::BlockBrown;

	WHY_LOG() << "Invalid block ID \"" << str << "\". Defaulting to \"BlockGrey\"";
	return ResourceId::BlockGrey;
}

why::RowAligment why::LevelLoader::str_to_row_aligment(const std::string &str) const
{
	if (str == "left") return RowAligment::Left;
	if (str == "center") return RowAligment::Center;
	if (str == "right") return RowAligment::Right;
	WHY_LOG() << "Invalid alignment value \"" << str << "\". Defaulting to \"center\"";
	return RowAligment::Center;
}

void why::LevelLoader::add_row(Row r, unsigned int repeat_row, const block_deque &blocks)
{
	for (unsigned int ridx = 0; ridx < repeat_row; ++ridx)
	{
		r.m_index = m_to.get_objects().size();
		for (auto b : blocks)
		{
			add_block(r, b.get<1>(), b.get<0>());
		}
	}
}

void why::LevelLoader::add_block(Row row, unsigned int repeat_block, block_dummy bd)
{
	for (unsigned int r = 0; r < repeat_block; ++r)
	{
		GameObjectBase *o = nullptr;

		if (bd.rcid == ResourceId::BlockTransparent)
		{
			o = new SpacerObject(m_rcm.get_sprite(ResourceId::BlockTransparent).get_size());
		}
		else if (bd.clone_from)
		{
			o = new BlockObject(*bd.clone_from);
			o->set_name("block_object");
		}
		else
		{
			o = new BlockObject(-1, &m_canvas,
				m_rcm.get_sprite(bd.rcid), 1, m_pc, m_sm, 
				(bd.name.empty() ? "block_object" : bd.name), bd.pos, bd.pos_parent);
		}
		m_to.add_object(row, o);
	}
}

////////////////////////////////////////////////////////
// Level
///////////////////////////////////////////////////////

unsigned int why::Level::m_max_level_time_sec = 30;

why::Level::Level(clan::Rectf area) : m_area(area), m_score(0), m_level_started(false),
m_paused(false), m_combo_started(false), m_combo_count(0)
{
	reset_score();
	//stop_timer();
}

why::Level::~Level()
{
	reset();
}

void why::Level::start()
{
	m_level_started = true;
	if (m_paused)
	{
		m_level_timer.resume();
		m_paused = false;
	}
	else
	{
		start_timer();
	}
}

void why::Level::stop()
{
	m_level_started = false;
	stop_timer();
}

void why::Level::pause()
{
	m_paused = true;
	stop_timer();
}

bool why::Level::is_started() const
{
	return m_level_started;
}

bool why::Level::is_paused() const
{
	return m_paused;
}

unsigned int why::Level::get_score() const
{
	return m_score;
}

void why::Level::reset_score()
{
	m_score = 0;
}

void why::Level::load(const std::string &path, clan::Canvas &c, const ResourceManager &rcm,
	clan::PhysicsContext &pc, const SettingsManager &sm)
{
	reset();
	LevelLoader(path, *this, c, rcm, pc, sm);
	set_positions();
	for (auto row : m_objects)
	{
		for (auto obj : row.second)
		{
			BlockObject *bo = dynamic_cast<BlockObject*>(obj);
			if (bo && rcm.get_id(bo->get_sprite()) == ResourceId::BlockGrey)
			{
				bo->enable_destruction(false);	
			}
		}
	}
}

clan::Rectf why::Level::get_area() const
{
	return m_area;
}

const why::level_object_row_map &why::Level::get_objects() const
{
	return m_objects;
}

void why::Level::add_object(Row r, GameObjectBase *obj)
{
	assert(obj);
	m_objects[r].push_back(obj);
}

float why::Level::get_row_width(unsigned int idx) const
{
	auto row_it = m_objects.find(Row(idx));
	if (row_it == m_objects.end() || row_it->second.empty())
	{
		return 0;
	}
	float ret = 0;
	for (auto &o : row_it->second)
	{
		ret += o->get_width();
	}
	return ret;
}

void why::Level::reset() 
{ 
	for (auto &o_pair : m_objects)
	{
		for (auto o : o_pair.second)
		{
			if (!Purgatory::is_added(o))
			{
				delete o;
			}
		}
	}
	m_combo_count = 0;
	m_combo_started = false;
	m_objects.clear();
	reset_score();
	m_falling_blocks.clear();
	m_level_timer.stop();
}

void why::Level::set_positions()
{
	using namespace clan;

	if (m_objects.empty()) return;

	Pointf pos;
	pos.y = m_area.top;
	bool new_row = true;

	for (auto &o_pair : m_objects)
	{
		new_row = true;

		if (o_pair.second.empty()) continue;

		if ((*o_pair.second.begin())->get_width() >= m_area.get_width())
		{
			pos.y += (*o_pair.second.begin())->get_height();
			continue;
		}

		float row_width = -1.0;
		for (auto o : o_pair.second)
		{
			const auto width = o->get_width();
			const auto height = o->get_height();

			auto *spacer = dynamic_cast<SpacerObject*>(o);
			if (spacer)
			{
				if (new_row)
				{
					pos.x = width + spacer->margin_left();
					pos.y += spacer->margin_bottom() + height + spacer->margin_top();
					new_row = false;
				}
				else
				{
					pos.x += width + spacer->margin_left() + spacer->margin_right();
				}
				continue;
			}

			auto *block = dynamic_cast<BlockObject*>(o);
			assert(block);

			if (row_width == -1)
			{
				row_width = get_row_width(o_pair.first.m_index);
			}

			if (block->m_pos_parent_item)
			{
				assert(block->m_pos != BlockPosition::Auto);

				if (block->m_pos == BlockPosition::AfterObject)
				{
					pos.x = block->m_pos_parent_item->get_position().x + block->m_pos_parent_item->get_width() +
						block->margin_left();

					if (pos.x > m_area.right)
					{
						pos.x = m_area.right - block->get_width() - block->margin_right();
					}
				}
				else
				{
					pos.x = block->m_pos_parent_item->get_position().x - o->get_width();
					if (pos.x < 0)
					{
						pos.x = block->margin_left();
					}
				}

				if (new_row)
				{
					pos.y += block->margin_bottom() + height + block->margin_top();
					new_row = false;
				}
			}

			else if (new_row)
			{
				pos.y += block->margin_bottom() + height + block->margin_top();

				switch (o_pair.first.m_aligment)
				{
				case RowAligment::Left:
				{
					pos.x = m_area.left;
					pos.x += m_area.get_width()*0.005f;
					break;
				}
				case RowAligment::Right:
				{
					pos.x = (m_area.right - row_width) - ((block->margin_left()*o_pair.second.size() +
						(block->margin_left() * o_pair.second.size())));
					pos.x -= m_area.get_width()*0.005f;
					break;
				}
				case RowAligment::Center:
				{
					pos.x = ((m_area.left + m_area.get_width() / 2) - row_width / 2) - (
						(block->margin_left() * o_pair.second.size()));
					break;
				}
				}
				new_row = false;
			}
			else
			{
				pos.x += block->margin_left();
			}

			block->set_position(pos);
			block->m_is_positioned = true;
			pos.x += width + block->margin_right();
		}
	}

	for (auto &row_it = m_objects.begin(); row_it != m_objects.end();)
	{
		GameObjectBasePtrDeque &objects = row_it->second;

		for (auto &obj_it = objects.begin(); obj_it != objects.end();)
		{
			if (dynamic_cast<const SpacerObject*>(*obj_it))
			{
				delete *obj_it;
				obj_it = objects.erase(obj_it );
			}
			else
			{
				++obj_it;
			}
		}

		if (row_it->second.empty())
		{
			m_objects.erase(row_it++);
		}
		else
		{
			++row_it;
		}
	}
}

void why::Level::draw(clan::Canvas &canvas)
{
	using namespace clan;

	for (auto &o_pair : m_objects)
	{
		for (auto o : o_pair.second)
		{
			o->draw(canvas);
		}
	}
}

void why::Level::update(clan::ubyte64 time_elapsed_ms)
{
	using namespace clan;

	if (m_objects.empty())return;

	for (auto &o_pair : m_objects)
	{
		for (auto it = o_pair.second.begin(); it != o_pair.second.end();)
		{
			bool erased = false;
			auto o = dynamic_cast<BlockObject*>(*it);

			if (o)
			{
				if (o->is_dead())
				{
					// If the block has dropped below game area
					if (o->get_position().y > m_area.bottom)
					{
						erased = true;
						Purgatory::add(o);
						it = o_pair.second.erase(it);
						increase_score();
						assert(!m_falling_blocks.empty());

						remove_falling_block(o);
					}
					else
					{
						// When dead, the blocks start to fall, so update position
						o->align_sprite_with_body();
						add_falling_block(o);
					}
					if (m_falling_blocks.size() > 1 && !m_combo_started)
					{
						++m_combo_count;
						m_combo_started = true;
					}
					else if (m_falling_blocks.empty() && m_combo_started)
					{
						m_combo_started = false;
					}
				}
			}
			if (!erased)
			{
				++it;
			}
		}
	}
}

void why::Level::add_falling_block(const BlockObject *o)
{
	// Check if the block is already added
	for (auto it = m_falling_blocks.begin(); it != m_falling_blocks.end(); ++it)
	{
		if (*it == o)
		{
			return;
		}
	}
	m_falling_blocks.push_back(o);
}

void why::Level::remove_falling_block(const BlockObject *o)
{
	auto it = m_falling_blocks.begin();
	for (it; it != m_falling_blocks.end(); ++it)
	{
		if (*it == o )
		{
			m_falling_blocks.erase(it);
			return;
		}
	}
	BOOST_ASSERT_MSG(it != m_falling_blocks.end(), "The object should always exist");
}

void why::Level::increase_score()
{
	const unsigned long elapsed_s = static_cast<long>(m_level_timer.elapsed().wall / 1000000000);
	int add = 0;
	if (elapsed_s > m_max_level_time_sec)
	{
		++add;
	}
	else
	{
		add += 1 * (m_max_level_time_sec - elapsed_s);
	}
	if (m_falling_blocks.size() > 1)
	{
		add *= m_falling_blocks.size();
	}
	m_score += add;
}

const why::GameObjectBase *why::Level::get_object(const std::string &name) const
{
	if (name.empty())return nullptr;

	for (auto row : m_objects)
	{
		for (auto o : row.second)
		{
			if (o->get_name() == name)
			{
				return o;
			}
		}
	}
	return nullptr;
}

bool why::Level::is_completed() const
{
	if (m_objects.empty())
	{
		return true;
	}
	if (!m_falling_blocks.empty())
	{
		return false;
	}
	bool all_blocks_gone = true;

	for (auto &o_pair : m_objects)
	{
		for (auto o : o_pair.second)
		{
			if (o->is_destroyable())
			{
				all_blocks_gone = false;
				break;
			}
		}
	}
	return all_blocks_gone;
}

void why::Level::start_timer()
{
	m_level_timer.start();
}

void why::Level::resume_timer()
{
	m_level_timer.resume();
}

void why::Level::stop_timer()
{
	m_level_timer.stop();
}

boost::tuple<int,int,int> why::Level::get_time_elapsed() const 
{
	long elapsed_s = static_cast<long>(m_level_timer.elapsed().wall / 1000000000);
	if (elapsed_s == 0)
	{
		return boost::make_tuple(0, 0, 0);
	}
	const int hour = elapsed_s / 3600;
	elapsed_s = elapsed_s % 3600;
	const int min = elapsed_s / 60;
	elapsed_s = elapsed_s % 60;
	return boost::make_tuple(hour, min, elapsed_s);
}

unsigned int why::Level::get_combo_count() const
{
	return m_combo_count;
}

void why::Level::reset_combo_count()
{
	m_combo_count = 0;
}