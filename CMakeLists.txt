#TODO: Replace Memory with name

CMake_Minimum_Required(VERSION 3.13)

Project(Memory)

If(DEFINED TESTING)
	If(${TESTING} STREQUAL true)
		Add_Executable(Memory "Test/Main.cpp")
		Target_Compile_Definitions(Memory PRIVATE TESTING)
		Add_Subdirectory(Test)
	Else()
		Add_Library(Memory STATIC "")
	EndIf()
	Unset(TESTING CACHE)
ELSE()
	Add_Library(Memory STATIC "")
EndIf()

Add_Subdirectory(Source)
Add_Subdirectory(Include)
Add_Subdirectory(Imp)

Target_Compile_Features(Memory 
    PUBLIC cxx_std_17)

Set(CMAKE_CXX_STANDARD "libc++")
#Set(CMAKE_CXX_FLAGS "-Wall" "-Wextra")

Install(TARGETS Memory EXPORT Memory DESTINATION lib/Memory)
Install(EXPORT Memory DESTINATION lib/Memory)
Install(FILES MemoryConfig.cmake DESTINATION lib/Memory)

#Fill in dependencies projects

Find_Package(Stream REQUIRED)
Target_Link_Libraries(Memory PUBLIC Stream)