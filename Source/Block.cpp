#include "Block.hpp"

namespace COMPANY_NAME
{
	namespace Memory
	{
		Block::~Block()
		{
			For_Each<Clean_Up>();
			items<Virtual_Functor<Clean_Up>>.clear();
		}
	} // namespace Memory
}