cmake_minimum_required(VERSION 3.22)
project(Desktop LANGUAGES CXX)

set(DESKTOP_SOURCES
        main.cpp
        MainWindow.cpp
        Pages/Settings/SettingsPage.cpp
        Pages/Home/HomePage.cpp
        Pages/Transactions/TransactionsPage.cpp
        Pages/NewTransaction/NewTransactionForm.cpp
        Dialogs/CustomFilters/CustomFiltersForm.cpp
        Dialogs/MultiSelect/MultiSelectDialog.cpp
        Dialogs/NewCategory/NewCategoryDialog.cpp
)

set(DESKTOP_HEADERS
        MainWindow.h
        Pages/Utils.h
        Pages/Settings/SettingsPage.h
        Pages/Home/HomePage.h
        Pages/Transactions/TransactionsPage.h
        Pages/NewTransaction/NewTransactionForm.h
        Dialogs/CustomFilters/CustomFiltersForm.h
        Dialogs/MultiSelect/MultiSelectDialog.h
        Pages/Transactions/MyComboBox.h
        Dialogs/NewCategory/NewCategoryDialog.h
)

set(DESKTOP_FORMS
        MainWindow.ui
        Pages/Settings/SettingsPage.ui
        Pages/Home/HomePage.ui
        Pages/Transactions/TransactionsPage.ui
        Pages/NewTransaction/NewTransactionForm.ui
        Dialogs/CustomFilters/CustomFiltersForm.ui
        Dialogs/MultiSelect/MultiSelectDialog.ui
        Dialogs/NewCategory/NewCategoryDialog.ui
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Network PrintSupport)

set(DESKTOP_RESOURCES
        Files/files.qrc
)

add_executable(Desktop
        ${DESKTOP_SOURCES}
        ${DESKTOP_HEADERS}
        ${DESKTOP_FORMS}
        ${DESKTOP_RESOURCES}
)

target_link_libraries(Desktop PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
        Qt6::Network
        Qt6::PrintSupport
        Backend
)

target_sources(Desktop PRIVATE
        QCustomPlot/qcustomplot.cpp
)
target_include_directories(Desktop PRIVATE
        ${CMAKE_SOURCE_DIR}/Backend
        ${CMAKE_SOURCE_DIR}/Backend/Modules
        ${CMAKE_SOURCE_DIR}/Desktop/QCustomPlot
)