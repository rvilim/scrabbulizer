cmake_minimum_required(VERSION 3.14)
project(scrabbulizer)

#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_executable(scrabbulizer main.cpp trie.cpp trie.h robin_hood.h)

#find_package ( OPENMP REQUIRED )

#target_link_libraries(${PROJECT_NAME} ${OpenMP_CXX_LIBRARIES}) # Link against it for C++
