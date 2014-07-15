#include "pch.hpp"
#include "why_purgatory.hpp"

std::deque < why::GameObjectBase * > why::Purgatory::m_purgatory;

void why::Purgatory::add(GameObjectBase *o)
{
	m_purgatory.push_back(o);
}

void why::Purgatory::send_to_hell()
{
	if (m_purgatory.empty())
	{
		return;
	}
	for (auto &o : m_purgatory)
	{
		if (o)
		{
			delete o;
			o = nullptr;
		}
	}
	m_purgatory.clear();
}

bool why::Purgatory::is_added(const GameObjectBase *o)
{
	assert(o);
	for (const auto &o2 : m_purgatory)
	{
		assert(o2);
		if (o == o2)return true;
	}
	return false;
}