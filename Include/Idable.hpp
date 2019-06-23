#pragma once
#include <Reflection/Initializable.hpp>
#include <Reflection/Operator_Helpers.hpp>
#include <Reflection/Variables.hpp>
#include <cstdint>

namespace COMPANY_NAME
{
	extern int object_id_count;

	using Id_Type = uint64_t;

	namespace Variable
	{
		using Id = class : public Base<Id_Type>
		{
		public:
			Variable_Type& Id();
			const Variable_Type& Id() const;
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
		//using Required_Features = Features<Operators::Comparable>;
		using Required_Constructs = Constructs<Initializable>;

		/*
			Summary:
				Sets the id to a unqiue id given
			Complexity:
				O(1)
		*/
		void Init();
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
	};

	template <typename Type>
	Id_Type Statically_Idable<Type>::id = 0;

	/*
		Args:
			other: The other object
		Return:
			True or false if the ids are the same
		Summary:
			Checks if the two ids are the same or not
		Complexity:
			O(1)
	*/
	template <typename Type1, typename Type2>
	bool operator<(const Idable<Type1>& idable1, const Idable<Type2>& idable2);

	template <typename Type1, typename Type2>
	bool operator<(const Statically_Idable<Type1>& idable1, const Statically_Idable<Type2>& idable2);
}

#include "Idable.ipp"