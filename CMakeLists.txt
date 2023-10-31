cmake_minimum_required(VERSION 3.10...999.999)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(little_adbcv)
add_definitions(-std=c++20)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB src_file ${CMAKE_SOURCE_DIR}/src/*.cc)
add_executable(little_adbcv ${CMAKE_SOURCE_DIR}/src/main.cc ${src_file})

target_link_libraries(little_adbcv ${OpenCV_LIBS})

if (WIN32 OR MSVC OR MSYS)
    add_custom_command(TARGET little_adbcv POST_BUILD
        COMMAND (if not exist "\"${CMAKE_SOURCE_DIR}/bin\"" mkdir "\"${CMAKE_SOURCE_DIR}/bin\"")
        COMMAND move little_adbcv.exe "\"${CMAKE_SOURCE_DIR}/bin/\""
    )
endif()
if (UNIX)
    add_custom_command(TARGET little_adbcv POST_BUILD
        COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/bin
        COMMAND mv little_adbcv ${CMAKE_SOURCE_DIR}/bin/
    )
endif()
