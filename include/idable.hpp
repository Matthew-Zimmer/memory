#pragma once
#include <reflection/initializable.hpp>
#include <reflection/operator_helpers.hpp>
#include <reflection/variables.hpp>
#include <cstdint>

namespace Slate
{
	extern int object_id_count;

	using Id_Type = std::size_t;

	namespace Variable
	{
		using Id = class : public Base<Id_Type>
		{
		public:
			Variable_Type& Id();
			Variable_Type const& Id() const;
		};
	}
	
	/*
		Variables:
			V::Id
		Constructs:
			Initializable
		Summary:
			This class will make type, Type, Idable
	*/
	template <typename Type>
	class Idable
	{
	public:
	
		using Required_Variables = Variables<V::Id>;
		using Required_Features = Features<Operators::Comparable>;
		using Required_Constructs = Constructs<Initializable>;

		/*
			Summary:
				Sets the id to a unique id given
			Complexity:
				O(1)
		*/

		void Init()
		{
			Meta::Cast<Type>(*this).Id() = object_id_count++;
		}

		template <typename Type1, typename Type2>
		friend bool operator<(Idable<Type1> const& idable1, Idable<Type2> const& idable2)
		{
			return Meta::Cast<Type1>(idable1).Id() < Meta::Cast<Type2>(idable2).Id();
		}

		
	};

	template <typename Type>
	class Statically_Idable
	{
		static Id_Type id;
	public:
		using Required_Features = Features<Operators::Comparable>;

		static Id_Type& Id()
		{
			return id;
		}

		template <typename Type1, typename Type2>
		friend bool operator<(Statically_Idable<Type1> const& idable1, Statically_Idable<Type2> const& idable2)
		{
			return Meta::Cast<Type1>(idable1).Id() < Meta::Cast<Type2>(idable2).Id();
		}
	};

	template <typename Type>
	Id_Type Statically_Idable<Type>::id = 0;
}