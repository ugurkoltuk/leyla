project(reversi-cpp)

cmake_minimum_required(VERSION 2.8)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-O3)
else()
    add_definitions(-g)
endif()

aux_source_directory(ai SRC_LIST)
aux_source_directory(game SRC_LIST)

if (UI_MODE STREQUAL "TUI")
    find_package(Curses REQUIRED)
    aux_source_directory(ui/tui SRC_LIST)
elseif(UI_MODE STREQUAL "NETWORK")
    find_package(Boost COMPONENTS system REQUIRED)
    include_directories(${Boost_INCLUDE_DIR})
    aux_source_directory(ui/network SRC_LIST)
endif()

include_directories(.)

add_executable(${PROJECT_NAME} ${SRC_LIST})
if (UI_MODE STREQUAL "TUI")
    target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})
elseif(UI_MODE STREQUAL "NETWORK")
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${Boost_SYSTEM_LIBRARY})
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
