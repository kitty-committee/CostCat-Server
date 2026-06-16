cmake_minimum_required(VERSION 3.31.11)

project (costcat)

set(CLIENTBUILD ON)

add_subdirectory(libs)

add_executable(costcat src/main.cpp)

target_sources(costcat PUBLIC
  src/db.cpp
  src/log_transaction.cpp
)

target_include_directories(costcat PUBLIC
  include
  libs/apiserver/include
  libs/apiserver/libs/mysql/include
  libs/apiserver/libs/httplib
  libs/AuthCat/include
)

target_link_libraries(costcat PUBLIC
  apiserver
  AuthCat-Client
)
