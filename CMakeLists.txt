cmake_minimum_required(VERSION 3.10)

project(strazer LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

if(UNIX)
  add_compile_options(-W -Wall -Wextra -Werror)
endif()

set(SOURCES
  src/strazer.cc
  src/strazer_filter.cc
  src/strazer_option.cc
  src/strazer_parser.cc
  src/strazer_syscall.cc)
add_executable(strazer ${SOURCES})
set_target_properties(strazer PROPERTIES CXX_STANDARD 17)
