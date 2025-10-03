#include "libbearpig/regexparser.h"
#include <libbearpig/lib.h>
#include <libbearpig/nfa.h>
#include <libbearpig/regexscanner.h>

int main(int argc, char** argv) {
  // std::string input("((a|abab)*b+)?");
  // std::string input("(a)");
  std::string input("(abcdef)[abc][^def]");
  if(argc > 1){
    input = argv[1];
  }
  RegexScanner scanner{input};
  auto tokens = scanner.tokenize();
  // for (auto token : tokens) {
  //   spdlog::info("token: {} ({}) at {}", token.data,
  //                to_string(token.tokentype), token.column);
  // }
  RegexParser regex_parser{tokens};
  if (regex_parser.parse()) {
    spdlog::info("successful parse!");
  }
  else {
    spdlog::warn("unsuccessful parse");
    spdlog::warn("current_token_idx: {}, tokenstream.size: {}", regex_parser.get_current_token_idx(), regex_parser.get_size_of_tokenstream());
    spdlog::warn("parser is {}", regex_parser.is_done() ? "done" : "not done");
  }

  NFA nfa;
  nfa.fill_with_dummy_data();
  nfa.to_dot();

  return 0;
}
