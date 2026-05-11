cmake_minimum_required(VERSION 4.2)
project(PacStation)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Strict conformance, no GNU/MSVC extensions
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Svg
        SvgWidgets
        REQUIRED)

set(SOURCES
        main.cpp
        mainwindow.h
        mainwindow.cpp
        sidebarbutton.h
        sidebarbutton.cpp
        thememanager.h
        thememanager.cpp
        iconutils.h
        iconutils.cpp
        resources.qrc
)

add_executable(pacstation ${SOURCES})
target_link_libraries(pacstation
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt6::Svg
        Qt6::SvgWidgets
)
