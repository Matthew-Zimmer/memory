#include <iostream>
#include <string>
#include <vector>
#include "Concept.hpp"

#include "Block.hpp"

namespace COMPANY_NAME
{
    namespace Test
    {
		constexpr double default_eps = 1.0 / 33554432.0;

		template <typename ... Tests>
		class Tester
		{
			static constexpr int count{ sizeof...(Tests) };
			class Comment_Checker
			{
			public:
				template <typename Type, typename ... Args>
				auto operator()(Type&& t, Args&& ... args) -> decltype(t.Comment());
			};
		public:
			void operator()()
			{
				if constexpr(static_cast<bool>(count))
				{
					int num_passed{ 0 };
					std::vector<std::string> failed_tests;
					([&]()
					{
						std::cout << "Testing " << Tests::Name() << ": ";
						if(Tests{}())
						{
							num_passed++;
							std::cout << "passed" << "\n";
						}
						else
						{
							if constexpr(Has_Function<Tests, Comment_Checker>)
								failed_tests.emplace_back(std::string{ Tests::Name() } + ", Comment: " + std::string{ Tests::Comment() });
							else
								failed_tests.emplace_back(Tests::Name());
							
							std::cout << "failed" << "\n";
						}
						
						if constexpr(Has_Function<Tests, Comment_Checker>)
							std::cout << "Comment: " << Tests::Comment() << "\n";
					}
					(),...);

					std::cout << "--------------" << "\n";
					std::cout << num_passed << "/" << count << " tests passed";
					std::cout << ", " << static_cast<double>(num_passed) / static_cast<double>(count) * 100.0 << "%" << "\n";

					if (failed_tests.size())
					{
						std::cout << "Failed test";
						if(failed_tests.size() > 1)
							std::cout << "s";
						std::cout << ":" << "\n";
						for(auto& x : failed_tests)
							std::cout << x << "\n";
					}
				}
			}
		};


		namespace Memory
		{

		}
	}
}