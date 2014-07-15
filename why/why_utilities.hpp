#pragma once

namespace why
{
	std::string get_os_info();

	// http://stackoverflow.com/questions/800955/remove-if-equivalent-for-stdmap
	namespace generic 
	{
		template <typename ContainerT, typename PredicateT>
		void erase_if(ContainerT& items, const PredicateT& predicate) 
		{
			for (auto it = items.begin(); it != items.end();) 
			{
				if (predicate(*it))
				{
					it = items.erase(it);
				}
				else
				{
					++it;
				}
			}
		};
	}
}