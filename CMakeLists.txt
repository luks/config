cmake_minimum_required(VERSION 3.9)

project(myconfig C)

set(CMAKE_C_STANDARD 99)

add_executable(myconfig main.c trie.c hashConf.c)

LINK_DIRECTORIES(/usr/local/lib/)

target_link_libraries (myconfig yaml config)

# pass preprocessor macros
#add_definitions(-DDEBUG)
