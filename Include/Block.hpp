#pragma once
#include <algorithm>
#include <memory>
#include <vector>
#include "Idable.hpp"

namespace COMPANY_NAME
{
	namespace Memory
	{
		class Block;
	}

	namespace Imp
	{
		namespace Concept
		{
			class Has_Arguments
			{
			public:
				template <typename Type>
				static auto Test(Type t) -> typename std::remove_reference_t<Type>::Argument_Types;
			};

			class Has_All_Behavoirs
			{
			public:
				template <typename Type>
				static auto Test(Type t) -> typename Type::All_Behavoirs;
			};
		}

		namespace Memory
		{
			template <typename Tag>
			class Function_Base
			{
			public:
				virtual ~Function_Base() = default;
				virtual void operator()(COMPANY_NAME::Memory::Block&, const void*) = 0;	
			};

			template <typename Type, typename Tag>
			class Function : public Function_Base<Tag>
			{
				Type function;
			private:
				template <typename ... Types>
				void Call(COMPANY_NAME::Memory::Block& b, const std::tuple<Types...>& args);
			public:
				Function() = default;
				Function(const Type& func);
				void operator()(COMPANY_NAME::Memory::Block& b, const void* data) final;
			};

			template <template <typename> typename Functor>
			class Virtual_Functor : public Is<Virtual_Functor<Functor>, Features<Idable>>
			{
				std::unique_ptr<Function_Base<Functor<void>>> func;
				using Is = Is<Virtual_Functor<Functor>, Features<Idable>>;
			public:
				Virtual_Functor();
				Virtual_Functor(std::unique_ptr<Function_Base<Functor<void>>>&& ptr);
				Virtual_Functor(const Virtual_Functor& f) = delete;
				Virtual_Functor(Virtual_Functor&& f);

				Virtual_Functor& operator=(const Virtual_Functor& f) = delete;
				Virtual_Functor& operator=(Virtual_Functor&& f);
				
				Function_Base<Functor<void>>& operator*();

				Function_Base<Functor<void>>* operator->();
			};
		}
	} // namespace Imp
	
	inline namespace Concept
	{
		template <typename Type>
		constexpr bool Has_Arguments = Satisfies<Imp::Concept::Has_Arguments, Type>;

		template <typename Type>
		constexpr bool Has_All_Behavoirs = Satisfies<Imp::Concept::Has_All_Behavoirs, Type>;
	}

	namespace Memory
	{
		template <template <typename> typename Functor>
		using Virtual_Functor = Imp::Memory::Virtual_Functor<Functor>;

		template <template <typename> typename Functor, typename Type, typename ... Args>
		Virtual_Functor<Functor> Make_Virtual_Functor(Args&& ... args);

		template <typename Type = void>
		class Clean_Up : public Is<Clean_Up<Type>, Features<Statically_Idable>>
		{
		public:
			void operator()(Block& block);
		};

		/*
			Summary:
				Manages all the vectors of different types
		*/
		class Block
		{
			/*
				Summary:
				The vectors for every different type
			*/
			template <typename Type>
			static std::vector<Type> items;

			template <typename Type, template <typename> typename ... Behavoirs>
			void Add_Behavoirs(Imp::Meta::_Types<Behavoirs<void>...>);

			template <typename Type, template <typename> typename ... Behavoirs>
			void Remove_Behavoirs(Imp::Meta::_Types<Behavoirs<void>...>);
		public:
			/*
				Summary:
					Clears all used vectors	
				Complexity:
					O(n_0 + ... + n_m) where n_0 is the lenght of the n_0 th vector, ..., 
					and n_m is the lenght of the n_m th vector 
			*/
			~Block();
			
			/*
				Summary:
					Clears the vector of type Type
				Complexity:
					O(n) where n is the lenght of vector<Type>
			*/
			template <typename Type>
			void Clear();
			
			/*
				Args:
					obj: The obj to be added
				Summary:
					Adds the object to the vector<Type>
				Complexity:
					O(1) on average
			*/
			template <typename Type>
			void Add(Type&& obj);
			
			/*
				Constraits:
					Type: Is_Idable<Type> is true
				Args:
					obj: The object to be removed
				Summary:
					Removes the object, obj from the vector
				Complexity:
					O(n) where n is the lenght of vector<Type>, worst case
			*/
			template <typename Type>
			void Remove(Type&& obj);
			/*
				Args:
					id: The id of the object to be removed
				Summary:
					Removes the object, which has the id, id from the vector
				Complexity:
					O(n) where n is the lenght of vector<Type>, worst case
			*/
			template <typename Type>
			void Remove(int id);

			/*
				Constraits:
					Functor: Is_Functor<Functor, Type> is true
				Args:
					func: The functor to check if an object should be removed
				Summary:
					Removes all objects which the functor evaluate to true
				Complexity:
					O(n) where n is the lenght of vector<Type>
			*/
			template <typename Type, typename Functor>
			void Remove_If(Functor&& func);

			/*
				Constraits:
					Type: C^1 compile time container
					Functor: Is_Functor<Functor, Type...> is true
				Args:
					f: The functor to call on each object in vector<Types...>
				Summary:
					Calls the functor, f, on all types in Type
				Complexity:
					O(n_0 * k_0 + ... + n_m * k_m) where n_0 is the lenght of the vector<Type_0>, 
						k_0 is the complexity of the functor with Type_0,
						n_m is the lenght of the vector<Type_m> and,
						k_m is the complexity of the functor with Type_m
			*/
			template <template <typename> typename Functor, typename Argument_Functor>
			void Complex_For_Each(Argument_Functor&& f);

			template <template <typename> typename Functor, typename ... Args>
			void For_Each(Args&& ... args);

			template <template <typename> typename Functor>
			void For_Each();
			
			/*
				Constraits:
					Type: Is_Idable<Type> is true
				Args:
					id: The id of the object
				Return:
					The object with the id, id	
				Summary:
					Gets the object with the id, id throws an expcetion if it is not found
				Complexity:
					O(lg n)
			*/
			template <typename Type>
			Type& Get(int id);

			template <typename Type>
			std::vector<Type>& Items();
		};
	}
}

#include "Block.ipp"	