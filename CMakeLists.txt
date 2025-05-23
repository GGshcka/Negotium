cmake_minimum_required(VERSION 3.28)
project(Negotium VERSION 0.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Xml Core Concurrent WebEngineWidgets)
find_package(Python3 COMPONENTS Development REQUIRED)
qt_standard_project_setup()

include_directories(${Python3_INCLUDE_DIRS})

file(GLOB files
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc"
)

qt_add_resources(RESOURCES resources.qrc)

qt_add_executable(${PROJECT_NAME} ${files} ${RESOURCES})

target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt6::Widgets
        PRIVATE Qt6::Xml
        PRIVATE Qt6::Core
        PRIVATE Qt6::Concurrent
        PRIVATE Qt6::WebEngineWidgets
        PRIVATE ${Python3_LIBRARIES}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE QT_NO_KEYWORDS)

set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
)