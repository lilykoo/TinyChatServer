set(SRC_LIST
main.cpp)

add_executable(ChatClient ${SRC_LIST})

target_link_libraries(ChatClient pthread)
