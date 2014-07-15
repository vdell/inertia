#include "pch.hpp"
#include "why_block_object.hpp"
#include "why_ball_object.hpp"

///////////////////////////////////////////////
// BlockObjectBase
///////////////////////////////////////////////

const float why::BlockObjectBase::m_default_margin_x = 1.5f;
const float why::BlockObjectBase::m_default_margin_y = 1.7f;

why::BlockObjectBase::BlockObjectBase() : m_is_positioned(false)
{

}

why::BlockObjectBase::~BlockObjectBase()
{

}

why::BlockObjectBase::BlockObjectBase(const BlockObjectBase &cpy)
{
	m_is_positioned = cpy.m_is_positioned;
}

float why::BlockObjectBase::margin_bottom() const
{
	return m_default_margin_y;
}

float why::BlockObjectBase::margin_top() const
{
	return m_default_margin_y;
}

float why::BlockObjectBase::margin_right() const
{
	return m_default_margin_x;
}

float why::BlockObjectBase::margin_left() const
{
	return m_default_margin_x;
}

//////////////////////////////////////////////
// BlockObject
//////////////////////////////////////////////

why::BlockObject::BlockObject(ResourceId id, clan::Canvas *canvas, clan::Sprite sprite,
	unsigned int health, clan::PhysicsContext &pc, const SettingsManager &sm,
	const std::string &name, BlockPosition pos, const BlockObject *pos_parent) :
	DestroyableObject(id, canvas, sprite, health, pc, name), clan::PolygonShape(pc),
	m_pos(pos), m_pos_parent_item(pos_parent)
{
	if (m_pos != BlockPosition::Auto)
	{
		assert(m_pos_parent_item);
	}
	using namespace clan;

	// There is some margin between blocks but the margin is there just for the looks. By increasing 
	// the size of the body the ball should not go between the blocks. 

	const float extra_size = 2.0; // px;

	set_as_box((get_width() + extra_size) / 2.0f, (extra_size + get_height()) / 2.0f);

	BodyDescription bd(pc);
	bd.set_type(BodyType::body_static);

	m_body = Body(pc, bd);
	m_body.set_data(this);

	FixtureDescription fd(pc);
	fd.set_density(sm.get_as_float("game.physics.block_density", 1.0f));
	fd.set_friction(sm.get_as_float("game.physics.block_friction", 0.0f));
	fd.set_restitution(sm.get_as_float("game.physics.block_restitution", 1.0f));
	fd.set_shape(*this);
	m_fixture = Fixture(pc, m_body, fd);
}

why::BlockObject::~BlockObject()
{

}

why::BlockObject::BlockObject(const BlockObject &cpy) : DestroyableObject(cpy),
clan::PolygonShape(cpy), BlockObjectBase(cpy)
{
}

void why::BlockObject::on_collision_begin(clan::Body &b)
{
	if (should_collide_with(b))
	{
		const BallObject *bo = dynamic_cast<BallObject*>(b.get_data());
		if (bo)
		{
			hit(bo->get_damage(), bo);
		}
	}
}
void why::BlockObject::on_collision_end(clan::Body &b)
{

}

bool why::BlockObject::should_collide_with(clan::Body &b)
{
	if (dynamic_cast<BlockObject*>(b.get_data()) || dynamic_cast<BallObject*>(b.get_data()))
	{
		return true;
	}
	return false;
}

unsigned int why::BlockObject::hit(unsigned int damage, const MovingObject *hitter)
{
	using namespace clan;
	const unsigned int health = DestroyableObject::hit(damage);

	// If the block is destroyed then make it fall through the floor.
	if (health == 0)
	{
		m_body.set_type(BodyType::body_dynamic);

		// Allows going through the floor
		m_fixture.set_as_sensor();
		m_body.set_gravity_scale(-1.0f);
		m_body.set_active(true);

		// Add random rotation to give some dynamic feeling.
		const float rotation = static_cast<float>((std::rand() % 11));

		set_rotation(Angle((((std::rand() % 2) == 0) ? -rotation : rotation),
			AngleUnit::angle_degrees));
	}
	return health;
}

//////////////////////////////////////////////
// SpacerObject
//////////////////////////////////////////////


why::SpacerObject::SpacerObject(clan::Size s, const std::string &name) :
GameObjectBase(ResourceId::BlockTransparent, name)
{
	m_size = s;
}

why::SpacerObject::~SpacerObject()
{

}

why::SpacerObject::SpacerObject(const SpacerObject &cpy) : GameObjectBase(cpy)
{
	m_size = cpy.m_size;
}

float why::SpacerObject::get_width() const
{
	return m_size.width;
}

float why::SpacerObject::get_height() const
{
	return m_size.height;
}

void why::SpacerObject::draw(clan::Canvas &c)
{

}