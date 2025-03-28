cmake_minimum_required(VERSION 3.16)

project(ImageViewer VERSION 1.0 LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS Quick)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options("/EHsc")
    add_compile_options("/utf-8")
endif()

if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib/win/opencv_world500d.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
    else()
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib/win/opencv_world500.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
    endif()
endif()

include(FetchContent)
FetchContent_Declare(
    spdlog
    URL ${CMAKE_CURRENT_SOURCE_DIR}/deps/spdlog-1.15.0.tar.gz
)
FetchContent_MakeAvailable(spdlog)

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

# 根据 CMAKE_SYSTEM_NAME 判断操作系统
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "System: Linux")
    find_package(OpenCV REQUIRED)

    # 检查是否找到 OpenCV
    if(OpenCV_FOUND)
        message(STATUS "OpenCV 库已找到.")
        message(STATUS "OpenCV 头文件路径: ${OpenCV_INCLUDE_DIRS}")
        message(STATUS "OpenCV 库文件路径: ${OpenCV_LIBS}")

        target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Quick spdlog::spdlog)
        include_directories(${PROJECT_NAME} ${OpenCV_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
    else()
        message(FATAL_ERROR "OpenCV 库未找到，请确保已安装 libopencv-dev 包并且 CMake 能够找到它。")
    endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "System: Windows")

    file(ARCHIVE_EXTRACT INPUT ${CMAKE_CURRENT_SOURCE_DIR}/lib/win.zip DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/win)
    target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/win)
    target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Quick spdlog::spdlog)

    target_link_libraries(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:opencv_world500d>"
        "$<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:opencv_world500>")
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE "$<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:TRUE>"
)
else()
    message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"     # 对于静态库 (.lib 或 .a)
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"    # 对于共享库 (.dll 或 .so)
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"    # 对于可执行文件
)
