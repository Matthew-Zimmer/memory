Get_Filename_Component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
Include(${SELF_DIR}/memory.cmake)


Find_Package(stream REQUIRED)