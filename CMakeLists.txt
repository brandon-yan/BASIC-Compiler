cmake_minimum_required(VERSION 3.15)
project(BASIC_Compiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(BASIC_Compiler main.cpp lexer.hpp ASTnode.hpp parser.hpp CFG.hpp)