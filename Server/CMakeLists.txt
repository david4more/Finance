#Server app
cmake_minimum_required(VERSION 3.22)
project(Server LANGUAGES CXX)

set(SERVER_SOURCES
        main.cpp
        mainwindow.cpp
)
set(SERVER_HEADERS
        mainwindow.h
)
set(SERVER_FORMS
        mainwindow.ui
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network HttpServer)

add_executable(Server
        ${SERVER_SOURCES}
        ${SERVER_HEADERS}
        ${SERVER_FORMS}
)

target_link_libraries(Server PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::HttpServer
)
