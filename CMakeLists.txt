cmake_minimum_required(VERSION 3.14)

project(Manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(sdbus-c++ REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

add_executable(Manager main.cc)
target_link_libraries(Manager PRIVATE sdbus-c++ ${SQLITE3_LIBRARIES})
target_include_directories(Manager PRIVATE ${SQLITE3_INCLUDE_DIRS})

find_program(CLANG_FORMAT_EXECUTABLE clang-format)

add_custom_target(
        generate_src
        COMMAND sdbus-c++-xml2cpp --adaptor=../adaptor.h ../adaptor.xml
        COMMENT "Generating .h file from .xml file with interface definition"
        VERBATIM
)

add_dependencies(Manager generate_src)

set(TO_FORMAT
        ../main.cc
        ../permissions.h)

if (CLANG_FORMAT_EXECUTABLE)
    add_custom_target(
            format
            COMMAND clang-format --style=Google --verbose -i ${TO_FORMAT}
            COMMENT "Running clang-format on source files"
            VERBATIM
    )
else ()
    message(WARNING "clang-format not found; format target will not be available.")
endif ()