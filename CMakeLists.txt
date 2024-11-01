cmake_minimum_required(VERSION 3.14)

project(Manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(sdbus-c++ REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

add_executable(Manager com/system/permissions/main.cc)
target_link_libraries(Manager PRIVATE sdbus-c++ ${SQLITE3_LIBRARIES})
target_include_directories(Manager PRIVATE ${SQLITE3_INCLUDE_DIRS})

add_executable(Time com/system/time/main.cc)
target_link_libraries(Time PRIVATE sdbus-c++)

add_executable(Main main.cc)
target_link_libraries(Main PRIVATE sdbus-c++)

find_program(CLANG_FORMAT_EXECUTABLE clang-format)

add_custom_target(
        generate_src
        COMMAND sdbus-c++-xml2cpp --adaptor=../com/system/permissions/permissions_adaptor.h ../com/system/permissions/permissions_adaptor.xml
        COMMAND sdbus-c++-xml2cpp --proxy=../com/system/permissions/permissions_proxy.h ../com/system/permissions/permissions_adaptor.xml
        COMMAND sdbus-c++-xml2cpp --adaptor=../com/system/time/time_adaptor.h ../com/system/time/time_adaptor.xml
        COMMAND sdbus-c++-xml2cpp --proxy=../com/system/time/time_proxy.h ../com/system/time/time_adaptor.xml
        COMMENT "Generating .h file from .xml file with interface definition"
        VERBATIM
)

add_dependencies(Manager generate_src)

set(TO_FORMAT
        ../com/system/permissions/main.cc
        ../com/system/time/main.cc
        ../com/system/permissions/permissions_service.h
        ../com/system/permissions/permissions_client.h
        ../com/system/time/time_service.h
        ../com/system/time/time_client.h
        ../main.cc)

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