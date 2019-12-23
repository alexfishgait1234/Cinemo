cmake_minimum_required(VERSION 3.10)
project(cinemo VERSION 1.0)

add_definitions(-O0 -g3 -Wall -std=c++11)
#aux_source_directory(. SRC)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(cinemo engine.cc)
#target_include_directories(${PROJECT_NAME} PUBLIC
                       #   "${PROJECT_BINARY_DIR}"
                       #   "${PROJECT_SOURCE_DIR}/include"
                        #  )

add_library(mp3lame STATIC IMPORTED)
if (WIN32)
  set_target_properties(mp3lame PROPERTIES IMPORTED_LOCATION lib/libmp3lame.a)
else()
  set_target_properties(mp3lame PROPERTIES IMPORTED_LOCATION /usr/local/lib/libmp3lame.a)
endif()
target_link_libraries(${PROJECT_NAME} mp3lame pthread)
