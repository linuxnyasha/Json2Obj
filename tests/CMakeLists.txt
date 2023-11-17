cmake_minimum_required(VERSION 3.27)
project(Json2Obj-Tests LANGUAGES CXX)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(Json2Obj)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)

FetchContent_MakeAvailable(Catch2)
file(GLOB SOURCES src/* src/*/* src/*/*/*)

add_executable(tests ${SOURCES})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain Json2Obj::Json2Obj nlohmann_json::nlohmann_json)
include(CTest)
include(Catch)
catch_discover_tests(tests)
