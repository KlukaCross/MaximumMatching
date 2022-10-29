cmake_minimum_required(VERSION 3.16)
project(MaximumMatching)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH "~/Qt/6.3.0/gcc_64/")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(MaximumMatching main.cpp main_window.cpp main_window.h node.h edge.h edge.cpp node.cpp field_drawing.cpp field_drawing.h)
target_link_libraries(MaximumMatching
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )

