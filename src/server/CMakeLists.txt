#定义了SRC_LIST变量包含了该目录下所有源文件
set(SRC_LIST 
chatserver.cpp 
chatservice.cpp
main.cpp
)

set(DB_LIST
db/db.cpp)

set(MODEL_LIST
model/usermodel.cpp
model/offlinemessagemodel.cpp
model/friendmodel.cpp
model/groupmodel.cpp)

set(REDIS_LIST
redis/redis.cpp)

add_executable(ChatServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST} ${REDIS_LIST})

#指定编译时需要依赖的库文件
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient pthread hiredis)