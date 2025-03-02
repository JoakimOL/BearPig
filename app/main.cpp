#include "libbearpig/regexparser.h"
#include <libbearpig/lib.h>
#include <libbearpig/nfa.h>
#include <libbearpig/regexscanner.h>

int main(int argc, char** argv) {
  // std::string input("((a|abab)*b+)?");
  std::string input("(a)");
  if(argc > 1){
    input = argv[1];
  }
  RegexScanner scanner{input};
  auto tokens = scanner.tokenize();
  RegexParser regex_parser{tokens};
  if (regex_parser.parse()) {
    spdlog::info("successful parse!");
    for (auto token : tokens) {
      spdlog::info("token: {} ({}) at {}", token.data,
                   to_string(token.tokentype), token.column);
    }
  }
  else {
    spdlog::warn("unsuccessful parse");
  }

  NFA nfa;
  nfa.fill_with_dummy_data();
  nfa.to_dot();

  return 0;
}
