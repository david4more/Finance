#Server app
cmake_minimum_required(VERSION 3.22)
project(Server LANGUAGES CXX)

set(SERVER_SOURCES
        main.cpp
        Server.cpp
)
set(SERVER_HEADERS
        Server.h
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Sql HttpServer)

add_executable(Server
        ${SERVER_SOURCES}
        ${SERVER_HEADERS}
)

target_link_libraries(Server PRIVATE
        Qt6::Core
        Qt6::Network
        Qt6::Sql
        Qt6::HttpServer
)
