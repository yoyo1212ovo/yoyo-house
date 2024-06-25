# 设置最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本
project(MyShell VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 将可执行文件添加到项目中
add_executable(myshell myshell.cpp)
