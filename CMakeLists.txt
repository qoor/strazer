cmake_minimum_required(VERSION 3.10)

project(strazer LANGUAGES CXX VERSION "0.0.1")

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

find_package(LLVM REQUIRED CONFIG)

message("Found LLVM ${LLVM_PACKAGE_VERSION}")
message("Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${CMAKE_BINARY_DIR} ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINATIONS})
llvm_map_components_to_libnames(LLVM_LIBS support)

set(SOURCES
  src/strazer.cc
  src/strazer_identifier_table.cc
  src/strazer_lexer.cc
  src/strazer_option.cc
  src/strazer_parser.cc
  src/strazer_token_kinds.cc
  src/strazer_utils.cc)

# include(CheckIncludeFiles)
# include(CheckSymbolExists)

# if(UNIX)
#   add_compile_options(-W -Wall -Wextra -Werror)

#   check_include_files(unistd.h HAVE_UNISTD_H)
#   if(NOT HAVE_UNISTD_H)
#     message(FATAL_ERROR "Cannot find the header unistd.h")
#   endif()

#   check_symbol_exists(getpagesize "unistd.h" HAVE_GETPAGESIZE)
#   check_symbol_exists(sysconf "unistd.h" HAVE_SYSCONF)

#   if (NOT (HAVE_GETPAGESIZE AND HAVE_SYSCONF))
#     message(FATAL_ERROR "Cannot get the page size on this machine")
#   endif()

#   list(APPEND SOURCES src/strazer_utils_unix.cc)
# endif()

configure_file(src/config.h.in ${CMAKE_BINARY_DIR}/config.h)

add_executable(strazer ${SOURCES})
set_target_properties(strazer PROPERTIES CXX_STANDARD 17)
target_link_libraries(strazer ${LLVM_LIBS})
