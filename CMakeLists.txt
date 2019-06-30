cmake_minimum_required(VERSION 3.14)
project(scrabbulizer)

set(CMAKE_CXX_STANDARD 14)

add_executable(scrabbulizer main.cpp trie.cpp trie.h robin_hood.h)