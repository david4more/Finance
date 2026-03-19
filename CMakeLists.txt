cmake_minimum_required(VERSION 3.22)

project(CoinWarden LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Backend)
add_subdirectory(Desktop)
add_subdirectory(Server)
