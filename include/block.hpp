#pragma once
#include <algorithm>
#include <memory>
#include <vector>

#include "idable.hpp"

namespace Slate
{
	namespace Memory
	{
		class Block;
	}

	namespace Imp::Concept
	{
		class Has_Arguments
		{
		public:
			template <typename Type>
			static auto Test(Type t) -> typename std::remove_reference_t<Type>::Argument_Types;
		};

		class Has_All_Behaviors
		{
		public:
			template <typename Type>
			static auto Test(Type t) -> typename Type::All_Behaviors;
		};
	}

	inline namespace Concept
	{
		template <typename Type>
		constexpr bool Has_Arguments = Satisfies<Imp::Concept::Has_Arguments, Type>;

		template <typename Type>
		constexpr bool Has_All_Behaviors = Satisfies<Imp::Concept::Has_All_Behaviors, Type>;
	}

	namespace Imp::Memory
	{
		template <typename Tag>
		class Function_Base
		{
		public:
			virtual ~Function_Base() = default;
			virtual void operator()(Slate::Memory::Block&, void const*) = 0;	
		};

		template <typename Type, typename Tag>
		class Function : public Function_Base<Tag>
		{
			Type function;
		private:
			template <typename ... Types>
			void Call(Slate::Memory::Block& b, std::tuple<Types...> const& args)
			{
				function(b, std::get<Types>(args)...);
			}
		public:
			Function() = default;
			Function(Type const& func) : function{ func }
			{}

			void operator()(Slate::Memory::Block& b, void const* data) final
			{
				if constexpr(Has_Arguments<Type>)
					Call(b, *reinterpret_cast<Meta::Convert_t<typename Type::Argument_Types, std::tuple> const*>(data));
				else
					function(b);
			}
		};

		template <template <typename> typename Functor>
		class Virtual_Functor : public Is<Virtual_Functor<Functor>, Features<Idable>>
		{
			std::unique_ptr<Function_Base<Functor<void>>> func;
			using Is = Is<Virtual_Functor<Functor>, Features<Idable>>;
		public:
			Virtual_Functor() : func{}
			{
				this->Init_All();
			}
			Virtual_Functor(std::unique_ptr<Function_Base<Functor<void>>>&& ptr) : func{ std::move(ptr) }
			{
				this->Init_All();
			}
			Virtual_Functor(Virtual_Functor const& f) = delete;
			Virtual_Functor(Virtual_Functor&& f) : Is{ V::Id{ f.Id() } }, func{ std::move(f.func) } 
			{}

			Virtual_Functor& operator=(Virtual_Functor const& f) = delete;
			Virtual_Functor& operator=(Virtual_Functor&& f)
			{
				func = std::move(f.func);
				this->Id() = f.Id();
				return *this;
			}
			
			Function_Base<Functor<void>>& operator*()
			{
				return func.operator*();
			}

			Function_Base<Functor<void>>* operator->()	
			{
				return func.operator->();
			}
		};
	}
	
	namespace Memory
	{
		inline namespace To_Move
		{
			class Unsubscribe
			{
			public:
				class Functor
				{
				public:
					template <typename Type>
					void operator()(Type&& t)
					{
						t.Unsubscribe();
					}
				};
				class Checker
				{
				public:
					template <typename Type>
					auto operator()(Type&& t) -> decltype(t.Unsubscribe());
				};
			};
		}

		template <template <typename> typename Functor>
		using Virtual_Functor = Imp::Memory::Virtual_Functor<Functor>;

		template <template <typename> typename Functor, typename Type, typename ... Args>
		Virtual_Functor<Functor> Make_Virtual_Functor(Args&& ... args)
		{
			Functor<Type>::Id() = object_id_count;
			return Virtual_Functor<Functor>{ std::make_unique<Imp::Memory::Function<Functor<Type>, Functor<void>>>(std::forward<Args>(args)...) };
		}

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

			template <typename Type, template <typename> typename ... Behaviors>
			void Add_Behaviors(Meta::Wrap<Behaviors<void>...>)
			{
				(Add<Virtual_Functor<Behaviors>>(Make_Virtual_Functor<Behaviors, Type>()), ...);
			}

			template <typename Type, template <typename> typename ... Behaviors>
			void Remove_Behaviors(Meta::Wrap<Behaviors<void>...>)	
			{
				(Remove<Virtual_Functor<Behaviors>>(Behaviors<Type>::Id()), ...);
			}
		public:
			/*
				Summary:
					Clears all used vectors	
				Complexity:
					O(n_0 + ... + n_m) where n_0 is the length of the n_0 th vector, ..., 
					and n_m is the length of the n_m th vector 
			*/
			~Block();
			
			/*
				Summary:
					Clears the vector of type Type
				Complexity:
					O(n) where n is the length of vector<Type>
			*/
			template <typename Type>
			void Clear()	
			{
				items<Type>.clear();
				//Remove the Clean_Up behavior
				if constexpr(!std::is_same_v<Type, Virtual_Functor<Clean_Up>>)
					Remove<Virtual_Functor<Clean_Up>>(Clean_Up<Type>::Id());
				//Remove other behaviors that Type declares it uses
				if constexpr(Has_All_Behaviors<Type>)
					if (!items<Type>.size())
						Remove_Behaviors<Type>(typename Type::All_Behaviors{});
			}
			
			/*
				Args:
					obj: The obj to be added
				Summary:
					Adds the object to the vector<Type>
				Complexity:
					O(1) on average
			*/
			template <typename Type>
			void Add(Type&& obj)	
			{
				//The non ref-qualifiad Type
				using Raw_Type = std::remove_reference_t<Type>;
				//Add the clean up functions if necessary only if it is not a clean up function itself
				if constexpr(!std::is_same_v<Raw_Type, Virtual_Functor<Clean_Up>>)
					if (!items<Raw_Type>.size())
						Add<Virtual_Functor<Clean_Up>>(Make_Virtual_Functor<Clean_Up, Raw_Type>());
				//Add other functions that Type declares it uses
				if constexpr(Has_All_Behaviors<Raw_Type>)
					if (!items<Raw_Type>.size())
						Add_Behaviors<Raw_Type>(typename Type::All_Behaviors{});
				//Add to the vector
				items<Raw_Type>.push_back(std::forward<Type>(obj));
			}
			
			/*
				Constraits:
					Type: Is_Idable<Type> is true
				Args:
					obj: The object to be removed
				Summary:
					Removes the object, obj from the vector
				Complexity:
					O(n) where n is the length of vector<Type>, worst case
			*/
			template <typename Type>
			void Remove(Type&& obj)
			{
				using Raw_Type = std::remove_reference_t<Type>;
				Remove<Raw_Type>(obj.Id());
			}
			/*
				Args:
					id: The id of the object to be removed
				Summary:
					Removes the object, which has the id, id from the vector
				Complexity:
					O(n) where n is the length of vector<Type>, worst case
			*/
			template <typename Type>
			void Remove(int id)
			{
				items<Type>.erase(std::lower_bound(items<Type>.begin(), items<Type>.end(), id, [](auto& x, auto& y) { return x.Id() < y; }));
				//Remove the clean up functions if necessary only if it is not a clean up function itself
				if constexpr(!std::is_same_v<Type, Virtual_Functor<Clean_Up>>)
					if (!items<Type>.size())
						Remove<Virtual_Functor<Clean_Up>>(Clean_Up<Type>::Id());
				//Remove other behaviors that Type declares it uses
				if constexpr(Has_All_Behaviors<Type>)
					if (!items<Type>.size())
						Remove_Behaviors<Type>(typename Type::All_Behaviors{});
			}

			/*
				Constraits:
					Functor: Is_Functor<Functor, Type> is true
				Args:
					func: The functor to check if an object should be removed
				Summary:
					Removes all objects which the functor evaluate to true
				Complexity:
					O(n) where n is the length of vector<Type>
			*/
			template <typename Type, typename Functor>
			void Remove_If(Functor&& func)
			{
				using Raw_Type = std::remove_reference_t<Type>;
				for (size_t i = 0; i < items<Raw_Type>.size(); i++)
					if (func(items<Raw_Type>[i]))
						items<Raw_Type>.erase(items<Raw_Type>.begin() + i--);
			}

			/*
				Constraits:
					Type: C^1 compile time container
					Functor: Is_Functor<Functor, Type...> is true
				Args:
					f: The functor to call on each object in vector<Types...>
				Summary:
					Calls the functor, f, on all types in Type
				Complexity:
					O(n_0 * k_0 + ... + n_m * k_m) where n_0 is the length of the vector<Type_0>, 
						k_0 is the complexity of the functor with Type_0,
						n_m is the length of the vector<Type_m> and,
						k_m is the complexity of the functor with Type_m
			*/
			template <template <typename> typename Functor, typename Argument_Functor>
			void Complex_For_Each(Argument_Functor&& f)	
			{
				for(auto& x : items<Virtual_Functor<Functor>>)
					f(x, *this);
			}

			template <template <typename> typename Functor, typename ... Args>
			void For_Each(Args&& ... args)	
			{
				auto tuple = std::make_tuple(std::forward<Args>(args)...);
				for(auto& x : items<Virtual_Functor<Functor>>)
					(*x)(*this, &tuple);
			}

			template <template <typename> typename Functor>
			void For_Each()	
			{
				for(auto& x : items<Virtual_Functor<Functor>>)
					(*x)(*this, nullptr);
			}
			
			/*
				Constraits:
					Type: Is_Idable<Type> is true
				Args:
					id: The id of the object
				Return:
					The object with the id, id	
				Summary:
					Gets the object with the id, id throws an exception if it is not found
				Complexity:
					O(lg n)
			*/
			template <typename Type>
			Type& Get(int id)
			{
				return *std::lower_bound(items<Type>.begin(), items<Type>.end(), id, [](auto& x, auto& y) {return x.Id() < y; });
			}

			template <typename Type>
			std::vector<Type>& Items()		
			{
				return items<Type>;
			}	
		};

		template <typename Type>
		std::vector<Type> Block::items;

		template <typename Type>
		void Clean_Up<Type>::operator()(Block& block)
		{
			block.Items<Type>().clear();
		}
	}
}