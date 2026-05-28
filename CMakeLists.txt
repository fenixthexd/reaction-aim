cmake_minimum_required(VERSION 3.16)
project(reaction_aim)

find_package(raylib REQUIRED)

add_executable(${PROJECT_NAME} WIN32 src/main.c)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")