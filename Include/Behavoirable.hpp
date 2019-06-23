#pragma once 
#include <Meta/Meta.hpp>
#include <Concept/Concept.hpp>

namespace COMPANY_NAME
{
	namespace Imp::Concept
	{
		class Has_Behavoirs
		{
		public:
			template <typename Type>
			static auto Test(Type) -> typename Type::Behavoirs;
		};
	}

	inline namespace Concept
	{
		template <typename Type>
		constexpr bool Has_Behavoirs = Satisfies<Imp::Concept::Has_Behavoirs, Type>;
	}

	namespace Memory
	{
		template <template <typename> typename Functor>
		using Behavoir = Functor<void>;

		template <typename Type>
		using Dependent_Behavoir = typename Type::Behavoir_Type;

		template <typename Type1, typename ... Types>
		class Behavoirable
		{
			template <typename _Type>
			class Builder
			{
			public:
				using Type = typename _Type::Behavoirs;
			};
		public:
			using All_Behavoirs = Meta::Unique<Meta::For_Each<Meta::Join<Meta::Use_If<Has_Behavoirs<Types>, Types>...>, Builder>>;
			using Generated_Behavoirs = All_Behavoirs;
		};
	}
}