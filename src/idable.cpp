#include "idable.hpp"

namespace Slate
{
	int object_id_count{ 1 };

	namespace Variable
	{
		Id::Variable_Type& Id::Id() 
		{
			return Variable();
		}	
		
		Id::Variable_Type const& Id::Id() const
		{
			return Variable();
		}		
	}
} // namespace Slate
