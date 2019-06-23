#TODO: Replace Memory with name

Target_Include_Directories(Memory PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> 
	$<INSTALL_INTERFACE:include/Memory>)

#TODO: Fill in .ipp files
Set	(Imps 
		Block.ipp
		Idable.ipp
	)

Install(FILES ${Imps} DESTINATION include/Memory)