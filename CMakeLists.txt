cmake_minimum_required(VERSION 3.13)
project(scrabbulizer)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

#FIND_PACKAGE( OpenMP REQUIRED)
#if(OPENMP_FOUND)
#    message("OPENMP FOUND")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#endif()

add_executable(scrabbulizer main.cpp trie.cpp trie.h scrabble.cpp scrabble.h)
add_executable(explain explainword.cpp trie.cpp trie.h scrabble.cpp scrabble.h )
add_executable(test test.cpp catch.h trie.cpp trie.h scrabble.cpp scrabble.h )