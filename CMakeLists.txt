cmake_minimum_required(VERSION 3.15)

project(
  bearpig
  VERSION 0.1
  DESCRIPTION "A scanner generator to be used with JACC"
  LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# only do if main project not if included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

  # ensure -std=c++xx instead of -std=g++xx
  set(cmake_cxx_extensions off)

  include(CTest)

endif()

include(FetchContent)

# fetch argparse
fetchcontent_declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG af442b4
)
fetchcontent_makeavailable(argparse)

fetchcontent_declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.2.0
)

fetchcontent_makeavailable(fmt)

fetchcontent_declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.3
)

fetchcontent_makeavailable(spdlog)

# library code is here
add_subdirectory(src)

# application code is here
add_subdirectory(app)

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) AND BUILD_TESTING)
  add_subdirectory(tests)
endif()

