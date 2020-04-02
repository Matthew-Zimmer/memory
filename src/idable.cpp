#include "idable.hpp"

namespace Slate
{
	int object_id_count{ 1 };

	namespace Variable
	{
		auto& Id::Id() 
		{
			return this->variable();
		}	
		
		auto const& Id::Id() const
		{
			return this->variable();
		}		
	}
} // namespace Slate
