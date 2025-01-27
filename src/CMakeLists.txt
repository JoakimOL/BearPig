set(HEADER_LIST "${project_SOURCE_DIR}/include/lib/lib.h")

add_library(lib lib.cpp ${HEADER_LIST})

target_include_directories(lib PUBLIC ../include)

fetchcontent_declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG 8e56133
)

fetchcontent_makeavailable(spdlog)

target_link_libraries(lib PRIVATE
  spdlog::spdlog
  argparse
)

# All users of this library will need at least C++20
target_compile_features(lib PUBLIC cxx_std_20)

