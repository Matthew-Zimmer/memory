#pragma once 
#include <concept/concept.hpp>
#include <meta/meta.hpp>

namespace Slate
{
	namespace Imp::Concept
	{
		class Has_Behaviors
		{
		public:
			template <typename Type>
			static auto Test(Type) -> typename Type::Behaviors;
		};
	}

	inline namespace Concept
	{
		template <typename Type>
		constexpr bool Has_Behaviors = Satisfies<Imp::Concept::Has_Behaviors, Type>;
	}

	namespace Memory
	{
		template <template <typename> typename Functor>
		using Behavior = Functor<void>;

		template <typename Type>
		using Dependent_Behavior = typename Type::Behavior_Type;

		template <typename Type1, typename ... Types>
		class Behavoirable
		{
			template <typename _Type>
			class Builder
			{
			public:
				using Type = typename _Type::Behaviors;
			};
		public:
			using All_Behaviors = Meta::Unique<Meta::For_Each<Meta::Join<Meta::Use_If<Has_Behaviors<Types>, Types>...>, Builder>>;
			using Generated_Behaviors = All_Behaviors;
		};
	}
}