cmake_minimum_required(VERSION 3.25)
project(picojson C)

set(CMAKE_C_STANDARD 11)

add_executable(picojson main.c picojson.c picojson.h)
