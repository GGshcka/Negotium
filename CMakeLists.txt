cmake_minimum_required(VERSION 3.28)
project(Negotium)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Xml Core Concurrent)
find_package(Python3 COMPONENTS Development REQUIRED)

include_directories(${Python3_INCLUDE_DIRS} #[[/usr/include/python3.13]])

file(GLOB files
        "${CMAKE_CURRENT_SOURCE_DIR}/*.*"
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Levels/*.*"
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Icons/*.*"
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Sprites/*.*"
)

add_executable(${PROJECT_NAME} ${files})

target_link_libraries(${PROJECT_NAME}
        Qt6::Widgets
        Qt6::Xml
        Qt6::Core
        Qt6::Concurrent
        ${Python3_LIBRARIES}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE QT_NO_KEYWORDS)