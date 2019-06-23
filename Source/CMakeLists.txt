#TODO: Replace Memory with name

#Fill in .cpp files
Target_Sources(Memory PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Block.cpp>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Idable.cpp>
	)