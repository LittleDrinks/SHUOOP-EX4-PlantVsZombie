cmake_minimum_required(VERSION 3.14)
project(ep4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC_FILES
    src/**.cpp
    include/**.h
    Engine/**.h
    Engine/**.cpp
    Game/**.h
    Game/**.cpp
)

add_executable(ep4_main ${SRC_FILES})
target_link_libraries(ep4_main libeasyx.a)
target_link_libraries(ep4_main winmm)
target_link_libraries(ep4_main Msimg32)