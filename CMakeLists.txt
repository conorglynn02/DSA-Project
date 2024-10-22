cmake_minimum_required(VERSION 3.10)

# Project name
project(BookIndexSearch)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add header files for Trie and HashTable
include_directories(include)

# Add executable for Indexer and Searcher
add_executable(indexer src/Indexer.cpp)
add_executable(searcher src/Searcher.cpp)

# Add any necessary libraries
# You can add linking libraries here if needed in the future