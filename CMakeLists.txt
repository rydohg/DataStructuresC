cmake_minimum_required(VERSION 3.15)
set(CMAKE_C_STANDARD 11)

#project(lab1)
#add_executable(lab1 labs/lab1.c)

project(hw1)
add_executable(hw1 hw1/hw1.c)

project(hw2)
add_executable(hw2 hw2/hw2.c)

project(hw3)
add_executable(hw3 hw3/hw3.c)

project(hw4)
add_executable(hw4 hw4/hw4.c)

project(hw4extra)
add_executable(hw4extra hw4/hw4_extra.c)

project(hw5)
add_executable(hw5 hw5/hw5.c hw5/hw5.h hw5/fakeRandHeight.c hw5/fakeRandHeight.h hw5/realRandHeight.h)

project(skiplist)
add_executable(skiplist hw5/skipList.c hw5/skipList.h hw5/fakeRandHeight.h hw5/fakeRandHeight.c)