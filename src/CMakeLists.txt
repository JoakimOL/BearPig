set(HEADER_LIST 
  "${bearpig_SOURCE_DIR}/include/libbearpig/lib.h"
  "${bearpig_SOURCE_DIR}/include/libbearpig/regexscanner.h"
  "${bearpig_SOURCE_DIR}/include/libbearpig/regextokens.h"
  "${bearpig_SOURCE_DIR}/include/libbearpig/parser.h"
  "${bearpig_SOURCE_DIR}/include/libbearpig/nfa.h"
)

add_library(libbearpig
   lib.cpp
   regexscanner.cpp
   regextokens.cpp
   parser.cpp
   nfa.cpp
   ${HEADER_LIST}
 )

target_include_directories(libbearpig PUBLIC ../include)

fetchcontent_declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG 8e56133
)

fetchcontent_makeavailable(spdlog)

fetchcontent_declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 9cf9f38
)

fetchcontent_makeavailable(spdlog)

target_link_libraries(libbearpig PRIVATE
  spdlog::spdlog
  fmt
  argparse
)

# All users of this library will need at least C++20
target_compile_features(libbearpig PUBLIC cxx_std_20)
