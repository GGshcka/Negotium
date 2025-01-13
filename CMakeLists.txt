cmake_minimum_required(VERSION 3.28)
project(Negotium)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(raylib REQUIRED)
find_package(Python3 COMPONENTS Development REQUIRED)

include_directories(${Python3_INCLUDE_DIRS} #[[/usr/include/python3.13]])

file(GLOB files
        "${CMAKE_CURRENT_SOURCE_DIR}/*.*"
)

add_executable(${PROJECT_NAME} ${files})

target_link_libraries(${PROJECT_NAME}
        Qt6::Widgets
        raylib
        ${Python3_LIBRARIES}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE QT_NO_KEYWORDS)