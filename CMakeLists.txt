cmake_minimum_required(VERSION 3.31.11)

project (costcat)

add_subdirectory(libs)

add_executable(costcat src/main.cpp)

target_include_directories(costcat PUBLIC
  include
  libs/apiserver/include
)

target_link_libraries(costcat PUBLIC
  apiserver
)
