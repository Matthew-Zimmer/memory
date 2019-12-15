#include "block.hpp"

namespace Slate::Memory
{
	Block::~Block()
	{
		For_Each<Clean_Up>();
		items<Virtual_Functor<Clean_Up>>.clear();
	}
}