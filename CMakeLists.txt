cmake_minimum_required(VERSION 3.7)

project(Netra)

if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)

    # list of files for which we add a copy rule
    set(data_SHADOW res/ship_1.png res/ship_2.png res/ship_3.png res/ship_4.png res/ship_5.png res/hack.ttf res/harald-body.png res/harald-arm.png res/rum.png)

    foreach(item IN LISTS data_SHADOW)
        message(STATUS ${item})
        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${item}"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${item}" "${CMAKE_CURRENT_BINARY_DIR}/${item}"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${item}"
        )
    endforeach()
endif()

add_custom_target(data-target-1 ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/res/ship_1.png")
add_custom_target(data-target-2 ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/res/ship_2.png")
add_custom_target(data-target-3 ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/res/ship_3.png")
add_custom_target(data-target-4 ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/res/ship_4.png")
add_custom_target(data-target-5 ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/res/ship_5.png")
add_custom_target(data-target-6 ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/res/hack.ttf")
add_custom_target(data-target-7 ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/res/harald-body.png")
add_custom_target(data-target-8 ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/res/harald-arm.png")
add_custom_target(data-target-9 ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/res/rum.png")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(SDL2_image REQUIRED)
include_directories(${SDL2_image_INCLUDE_DIRS})

find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_ttf_INCLUDE_DIRS})

find_package(fmt REQUIRED)

add_executable(netra src/main.cpp src/events.cpp src/timer.cpp src/objects.cpp src/ringbuffer.cpp src/tools.cpp)
target_include_directories(netra PRIVATE include/)
target_link_libraries(netra SDL2 SDL2_image SDL2_ttf fmt::fmt)
