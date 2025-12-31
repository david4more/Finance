# Backend library
project(Backend LANGUAGES CXX)

set(BACKEND_SOURCES
        Backend.cpp
        Modules/Model.cpp
        Managers/TransactionsManager.cpp
        Managers/CurrenciesManager.cpp
        Managers/CategoriesManager.cpp
        Managers/AccountsManager.cpp
        Managers/NetworkManager.cpp
)

set(BACKEND_HEADERS
        Backend.h
        Modules/Model.h
        Managers/TransactionsManager.h
        Managers/CurrenciesManager.h
        Managers/CategoriesManager.h
        Managers/AccountsManager.h
        Managers/NetworkManager.h
        Modules/Category.h
        Modules/Transaction.h
        Modules/Utils.h
        Modules/Currency.h
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_library(Backend SHARED ${BACKEND_SOURCES} ${BACKEND_HEADERS})

target_precompile_headers(Backend PRIVATE Modules/pch.h)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Network)
target_link_libraries(Backend PRIVATE Qt6::Core Qt6::Sql Qt6::Network)

target_compile_definitions(Backend PRIVATE BACKEND_LIB)
