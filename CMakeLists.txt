cmake_minimum_required(VERSION 3.14)
project(ep4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE CORE_FILES
    Engine/**.h
    Engine/**.cpp
    Game/**.h
    Game/**.cpp
)

add_library(ep4_core ${CORE_FILES})
target_include_directories(ep4_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ep4_main src/main.cpp)
target_link_libraries(ep4_main ep4_core libeasyx.a winmm Msimg32)

# ----------------
# Unit tests (GoogleTest)
# ----------------
enable_testing()

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
FetchContent_Declare(
  googletest
  URL http://cdn.tarsyun.com/src/release-1.11.0.tar.gz
)

# For MSVC: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(ep4_tests
    tests/test_controller_input.cpp
    tests/test_ui_layers.cpp
    tests/test_sun_collect_math.cpp
    tests/test_grid_mapping.cpp
    tests/test_text_color.cpp
    tests/test_plant_enabled_damage.cpp
)
target_link_libraries(ep4_tests PRIVATE ep4_core GTest::gtest_main libeasyx.a winmm Msimg32)

include(GoogleTest)
# Avoid running test discovery at build time (some environments may hang).
if(WIN32)
  add_test(
    NAME ep4_tests
    COMMAND powershell -NoProfile -ExecutionPolicy Bypass -Command "& \"$<TARGET_FILE:ep4_tests>\" *> \"ep4_tests.log\"; exit $LASTEXITCODE"
  )
  set_tests_properties(ep4_tests PROPERTIES WORKING_DIRECTORY $<TARGET_FILE_DIR:ep4_tests>)
else()
  add_test(NAME ep4_tests COMMAND ep4_tests)
endif()