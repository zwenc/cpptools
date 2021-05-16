CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(UnitTest)

SET(CMAKE_CXX_COMPILER "g++")         #设置编译器（可选的有gcc，g++）

# 设置用debug还是release模式。debug允许断点，而release更快
#set( CMAKE_BUILD_TYPE Debug )
set( CMAKE_BUILD_TYPE Release )

# 设置编译选项
# 允许c++11标准、O3优化、多线程。
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -O3 -pthread" )

INCLUDE_DIRECTORIES(../)

ADD_EXECUTABLE(test thread_test.cpp)

