cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "C:/Users/FYM/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project(warehouse_electronics CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets CONFIG REQUIRED COMPONENTS core base)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

file(GLOB_RECURSE SRC_FILES
    src/*.cpp
)

add_executable(warehouse_app WIN32 ${SRC_FILES})

target_include_directories(warehouse_app PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(warehouse_app
    PRIVATE
        wx::core
        wx::base
        unofficial::sqlite3::sqlite3
)