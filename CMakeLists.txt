cmake_minimum_required(VERSION 3.6)
project(ve482p1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    .gitignore
    main.c
    README.txt shell.c cmds.c cmds.h)

add_executable(ve482p1 ${SOURCE_FILES})