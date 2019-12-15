cmake_minimum_required(VERSION 3.13)

project(memory)

add_library(memory STATIC "")

add_subdirectory(include)
add_subdirectory(src)

target_compile_features(memory PUBLIC cxx_std_17)

#Set(CMAKE_CXX_FLAGS "-Wall" "-Wextra")

install(TARGETS memory EXPORT memory DESTINATION lib/memory)
install(EXPORT memory DESTINATION lib/memory)
install(FILES memory-config.cmake DESTINATION lib/memory)

#Fill in dependencies projects

find_package(stream REQUIRED)
target_link_libraries(memory PUBLIC stream)