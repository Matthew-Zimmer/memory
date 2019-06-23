#TODO: Replace Memory with name

Get_Filename_Component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
Include(${SELF_DIR}/Memory.cmake)


#TODO: Fill in dependencies projects
Find_Package(Stream REQUIRED)