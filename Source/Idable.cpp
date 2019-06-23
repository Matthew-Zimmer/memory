#include "Idable.hpp"

namespace COMPANY_NAME
{
	int object_id_count{ 1 };

	namespace Variable
	{
		Id::Variable_Type& Id::Id() 
		{
			return Variable();
		}	
		
		const Id::Variable_Type& Id::Id() const
		{
			return Variable();
		}		
	}
} // namespace COMPANY_NAME::Memory
