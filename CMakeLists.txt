#cmake_minimum_required(VERSION 3.14)
#project(scrabbulizer)
#
##set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fopenmp")
#
#add_executable(scrabbulizer main.cpp trie.cpp trie.h robin_hood.h scrabble.cpp scrabble.h)
#add_executable(test test.cpp trie.cpp trie.h robin_hood.h scrabble.cpp scrabble.h)
#
#find_package ( OPENMP REQUIRED )
#
#target_link_libraries(${PROJECT_NAME} ${OpenMP_CXX_LIBRARIES}) # Link against it for C++

cmake_minimum_required(VERSION 3.13)
project(scrabbulizer)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(SOURCE_FILES main.cpp trie.cpp trie.h robin_hood.h scrabble.cpp scrabble.h)
set(SOURCE_FILES explainword.cpp trie.cpp trie.h scrabble.cpp scrabble.h )
add_executable(scrabbulizer ${SOURCE_FILES})
add_executable(explain ${SOURCE_FILES})