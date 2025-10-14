#include <exception>
#include <libbearpig/lib.h>
#include <libbearpig/nfa.h>
#include <libbearpig/regexscanner.h>
#include <libbearpig/regexparser.h>

#include <argparse/argparse.hpp>

int main(int argc, char** argv) {
  // std::string input("((a|abab)*b+)?");
  // std::string input("(a)");
  std::string input(R"([abc]\[)");
  argparse::ArgumentParser program(argv[0]);
  program.add_argument("input").help("input regex to use").remaining();
  program.add_argument("-v").flag().help("enable verbose logging");

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& e) {
    spdlog::error(e.what());
    exit(1);
  }

  if(program.is_used("-v")){
    spdlog::set_level(spdlog::level::debug);
  }

  if(program.is_used("input")){
    input = program.get<std::string>("input");
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
