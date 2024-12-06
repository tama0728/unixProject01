cmake_minimum_required(VERSION 3.28)
project(Server C)

set(CMAKE_C_STANDARD 99)

# GTK 찾기 (pkg-config 사용)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)

# GTK 관련 플래그 추가
include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})
add_definitions(${GTK_CFLAGS_OTHER})

add_executable(${PROJECT_NAME} server.c gameLogic.c gameLogic.h)

target_link_libraries(${PROJECT_NAME} ${GTK_LIBRARIES})
