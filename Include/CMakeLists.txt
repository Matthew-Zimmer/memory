#TODO: Replace Memory with name

Target_Include_Directories(Memory PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> 
	$<INSTALL_INTERFACE:include/Memory>)

#TODO: Fill in .hpp Files
Set	(Headers 
		Behavoirable.hpp
		Block.hpp
		Idable.hpp
	)

Install(FILES ${Headers} DESTINATION include/Memory)