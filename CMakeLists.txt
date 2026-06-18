cmake_minimum_required(VERSION 3.31.11)

project (costcat)

set(CLIENTBUILD ON)

add_subdirectory(libs)

add_executable(costcat src/main.cpp)

target_sources(costcat PUBLIC
  src/db.cpp
  src/json.cpp
  src/log_transaction.cpp
  src/auth.cpp
  src/basic_responses.cpp
  src/simplify_with_collector.cpp
  src/zero_subset_sum.cpp
  src/get_balances.cpp
  src/get_debts.cpp
)

target_include_directories(costcat PUBLIC
  include
  libs/apiserver/include
  libs/apiserver/libs/mysql/include
  libs/apiserver/libs/httplib
  libs/AuthCat/include
  libs/AuthCat/libs/json/include
)

target_link_libraries(costcat PUBLIC
  apiserver
  AuthCat-Client
  nlohmann_json::nlohmann_json
)

install (TARGETS costcat)
