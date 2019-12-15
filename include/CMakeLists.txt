Target_Include_Directories(memory PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> 
	$<INSTALL_INTERFACE:include/memory>)


Set(headers 
	behavoirable.hpp
	block.hpp
	idable.hpp
)

Install(FILES ${headers} DESTINATION include/memory)
