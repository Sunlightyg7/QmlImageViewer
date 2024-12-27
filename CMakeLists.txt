cmake_minimum_required(VERSION 3.16)

project(ImageViewer VERSION 1.0 LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS Quick)

set(CMAKE_CXX_STANDARD 17)
set(OPENCV_LIB_DIR_PATH $ENV{OPENCV_LIB_DIR})
add_compile_options("/EHsc")

message(STATUS ${OPENCV_LIB_DIR_PATH})

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/include/*.h)
file(GLOB RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 创建可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

target_link_directories(${PROJECT_NAME} PUBLIC ${OPENCV_LIB_DIR_PATH})
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Quick opencv_world500d)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include)