#pragma once
#include <exception>
#include <iostream>

namespace COMPANY_NAME 
{
	namespace Imp
	{
		namespace Memory
		{
			template <typename Type, typename Tag>
			Function<Type, Tag>::Function(const Type& func) : function{ func }
			{}

			template <typename Type, typename Tag>
			void Function<Type, Tag>::operator()(COMPANY_NAME::Memory::Block& b, const void* data)
			{
				if constexpr(Has_Arguments<Type>)
					Call(b, *reinterpret_cast<const Meta::Convert_t<typename Type::Argument_Types, std::tuple>*>(data));
				else
					function(b);
			}

			template <typename Type, typename Tag>
			template <typename ... Types>
			void Function<Type, Tag>::Call(COMPANY_NAME::Memory::Block& b, const std::tuple<Types...>& args)
			{
				function(b, std::get<Types>(args)...);
			}
			
			template <template <typename> typename Functor>
			Virtual_Functor<Functor>::Virtual_Functor() : func{}
			{
				this->Init_All();
			}
			
			template <template <typename> typename Functor>
			Virtual_Functor<Functor>::Virtual_Functor(std::unique_ptr<Function_Base<Functor<void>>>&& ptr) : func{ std::move(ptr) }
			{
				this->Init_All();
			}
			
			template <template <typename> typename Functor>
			Virtual_Functor<Functor>::Virtual_Functor(Virtual_Functor&& f) : Is{ V::Id{ f.Id() } }, func{ std::move(f.func) } 
			{}

			template <template <typename> typename Functor>
			Virtual_Functor<Functor>& Virtual_Functor<Functor>::operator=(Virtual_Functor&& f)
			{
				func = std::move(f.func);
				this->Id() = f.Id();
				return *this;
			}

			template <template <typename> typename Functor>
			Function_Base<Functor<void>>& Virtual_Functor<Functor>::operator*()
			{
				return func.operator*();
			}

			template <template <typename> typename Functor>
			Function_Base<Functor<void>>* Virtual_Functor<Functor>::operator->()
			{
				return func.operator->();
			}

			
		}
	}

	namespace Memory
	{
		template <template <typename> typename Functor, typename Type, typename ... Args>
		Virtual_Functor<Functor> Make_Virtual_Functor(Args&& ... args)
		{
			Functor<Type>::Id() = object_id_count;
			return Virtual_Functor<Functor>{ std::make_unique<Imp::Memory::Function<Functor<Type>, Functor<void>>>(std::forward<Args>(args)...) };
		}

		template <typename Type>
		void Clean_Up<Type>::operator()(Block& b)
		{
			b.Items<Type>().clear();
		}

		template <typename Type, template <typename> typename ... Behavoirs>
		void Block::Add_Behavoirs(Imp::Meta::_Types<Behavoirs<void>...>)
		{
			(Add<Virtual_Functor<Behavoirs>>(Make_Virtual_Functor<Behavoirs, Type>()), ...);
		}

		template <typename Type, template <typename> typename ... Behavoirs>
		void Block::Remove_Behavoirs(Imp::Meta::_Types<Behavoirs<void>...>)
		{
			(Remove<Virtual_Functor<Behavoirs>>(Behavoirs<Type>::Id()), ...);
		}

		template <typename Type>
		std::vector<Type> Block::items;

		template <typename Type>
		void Block::Clear()
		{
			items<Type>.clear();
			//Remove the Clean_Up behavoir
			if constexpr(!std::is_same_v<Type, Virtual_Functor<Clean_Up>>)
				Remove<Virtual_Functor<Clean_Up>>(Clean_Up<Type>::Id());
			//Remove other behavoirs that Type declares it uses
			if constexpr(Has_All_Behavoirs<Type>)
				if (!items<Type>.size())
					Remove_Behavoirs<Type>(typename Type::All_Behavoirs{});
		}

		template <typename Type>
		void Block::Add(Type&& obj)
		{
			//The non ref-qualifiad Type
			using Raw_Type = std::remove_reference_t<Type>;
			//Add the clean up functions if nessarry only if it is not a clean up function itself
			if constexpr(!std::is_same_v<Raw_Type, Virtual_Functor<Clean_Up>>)
				if (!items<Raw_Type>.size())
					Add<Virtual_Functor<Clean_Up>>(Make_Virtual_Functor<Clean_Up, Raw_Type>());
			//Add other functions that Type declares it uses
			if constexpr(Has_All_Behavoirs<Raw_Type>)
				if (!items<Raw_Type>.size())
					Add_Behavoirs<Raw_Type>(typename Type::All_Behavoirs{});
			//Add to the vector
			items<Raw_Type>.push_back(std::forward<Type>(obj));
		}
		
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

		template <typename Type>
		void Block::Remove(Type&& obj)
		{
			using Raw_Type = std::remove_reference_t<Type>;
			Remove<Raw_Type>(obj.Id());
		}
		
		template <typename Type>
		void Block::Remove(int id)
		{
			items<Type>.erase(std::lower_bound(items<Type>.begin(), items<Type>.end(), id, [](auto& x, auto& y) { return x.Id() < y; }));
			//Remove the clean up functions if nessarry only if it is not a clean up function itself
			if constexpr(!std::is_same_v<Type, Virtual_Functor<Clean_Up>>)
				if (!items<Type>.size())
					Remove<Virtual_Functor<Clean_Up>>(Clean_Up<Type>::Id());
			//Remove other behavoirs that Type declares it uses
			if constexpr(Has_All_Behavoirs<Type>)
				if (!items<Type>.size())
					Remove_Behavoirs<Type>(typename Type::All_Behavoirs{});
		}

		template <typename Type, typename Functor>
		void Block::Remove_If(Functor&& func)
		{
			using Raw_Type = std::remove_reference_t<Type>;
			for (size_t i = 0; i < items<Raw_Type>.size(); i++)
				if (func(items<Raw_Type>[i]))
					items<Raw_Type>.erase(items<Raw_Type>.begin() + i--);
		}

		template <template <typename> typename Functor, typename Argument_Functor>
		void Block::Complex_For_Each(Argument_Functor&& f)
		{
			for(auto& x : items<Virtual_Functor<Functor>>)
				f(x, *this);
		}

		template <template <typename> typename Functor, typename ... Args>
		void Block::For_Each(Args&& ... args)
		{
			auto tuple = std::make_tuple(std::forward<Args>(args)...);
			for(auto& x : items<Virtual_Functor<Functor>>)
				(*x)(*this, &tuple);
		}

		template <template <typename> typename Functor>
		void Block::For_Each()
		{
			for(auto& x : items<Virtual_Functor<Functor>>)
				(*x)(*this, nullptr);
		}
		
		template <typename Type>
		Type& Block::Get(int id)
		{
			auto iter = std::lower_bound(items<Type>.begin(), items<Type>.end(), id, [](auto& x, auto& y) {return x.Id() < y; });
			if (iter != items<Type>.end())
				return *iter;
			throw std::domain_error{ "Could not find Object with id: " + std::to_string(id) };
		}

		template <typename Type>
		std::vector<Type>& Block::Items()
		{
			return items<Type>;
		}	
	}
}