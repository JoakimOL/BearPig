set(HEADER_LIST 
  "${bearpig_SOURCE_DIR}/include/libbearpig/regexscanner.h"
  "${bearpig_SOURCE_DIR}/include/libbearpig/regextokens.h"
  "${bearpig_SOURCE_DIR}/include/libbearpig/regexparser.h"
  "${bearpig_SOURCE_DIR}/include/libbearpig/regexast.h"
  "${bearpig_SOURCE_DIR}/include/libbearpig/nfa.h"
  "${bearpig_SOURCE_DIR}/include/libbearpig/basevisitor.h"
  "${bearpig_SOURCE_DIR}/include/libbearpig/printvisitor.h"
  "${bearpig_SOURCE_DIR}/include/libbearpig/nfagenvisitor.h"
)

add_library(libbearpig
   regexscanner.cpp
   regextokens.cpp
   regexparser.cpp
   regexast.cpp
   nfa.cpp
   printvisitor.cpp
   nfagenvisitor.cpp
   ${HEADER_LIST}
 )

target_include_directories(libbearpig PUBLIC ../include)

target_link_libraries(libbearpig PRIVATE
  spdlog::spdlog
  fmt
  argparse
)

# All users of this library will need at least C++20
target_compile_features(libbearpig PUBLIC cxx_std_20)
