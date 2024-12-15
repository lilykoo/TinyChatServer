cmake_minimum_required(VERSION 3.0)
project(main) 
#该工程文件名

#配置编译选项-2
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#配置头文件搜索路径-4
include_directories(${PROJECT_SOURCE_DIR}/include())

#配置库文件搜索路径-5
#link_directories()

#生成文件以及需要编译的相关源文件-1，3
#设置需要编译的源文件列表-3
#./... 表示当前文件夹（CMakeLists.txt）下的某个源文件 
set(SRC_LIST ./httpcontext.cpp
./httprequest.cpp
./httpresponse.cpp
./server.cpp
./util.cpp
./main.cpp
)

#把...指定路径下源文件名放入变量名SRC_LIST中
#aux_source_directory(... SRC_LIST)

add_executable(server ${SRC_LIST})

#设置可执行文件最终存放路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#连接相关库-6，连接相关头文件
target_link_libraries(server muduo_net muduo_base pthread)

