//#include "Tests.hpp"
#include <iostream>
#include "Block.hpp"
#include "Idable.hpp"
#include "Behavoirable.hpp"

using namespace COMPANY_NAME;
using namespace Memory;

class A_
{
public:
	int id;
	A_(int id) : id{id}{}
};

template <typename Type>
class Update : public Is<Update<Type>, Features<Statically_Idable>>
{
public:
	using Argument_Types = Meta::Wrap<A_>;
	void operator()(Block& b, A_ num)
	{
		for(auto& x : b.Items<Type>())
			x.Update(num);
	}
};

template <typename Type>
class Updatable
{
public:
	
	using Behavoirs = Meta::Wrap<Behavoir<::Update>>;
};

template <typename Type>
class Drawable
{
public:
	
	//using Behavoirs = Meta::Wrap<Behavoir<::Draw>>;
};

class A : public Is<A, Features<Idable, Updatable>, Constructs<Behavoirable>>
{
public:
	void Update(A_ num)
	{
		std::cout << "Hello, world: # 00" << num.id << "\n";
	}

	//using Behavoirs = Meta::Wrap<Behavoir<::Draw>>;
	//using All_Behavoirs = Meta::Unique<Behavoirs, Generated_Behavoirs>;
};


int main(int argc, char** argv)
{
	//using namespace COMPANY_NAME::Test;

	// Tester<
	// 	#include "Tests"
	// >{}();
	

	Block b;
	b.Add(A{});
	
	b.Complex_For_Each<Update>([](auto& x, Block& b){auto t = std::make_tuple(A_{ 7 }); (*x)(b, &t); });

	
	//b.Remove<A>(0);
	//b.Items<Virtual_Functor<Clean_Up>>()[0]->operator()(b, nullptr);
	std::cout << "----" << "\n";

	std::cout << b.Items<Virtual_Functor<Clean_Up>>().size() << "\n";
	std::cout << b.Items<Virtual_Functor<Update>>().size() << "\n";
	//std::cout << b.Items<Virtual_Functor<Draw>>().size() << "\n";
	std::cout << b.Items<A>().size() << "\n";

	std::cout << "----" << "\n";

	return 0;
}