#include "libbearpig/nfagenvisitor.h"
#include "libbearpig/printvisitor.h"
#include <exception>
#include <libbearpig/lib.h>
#include <libbearpig/nfa.h>
#include <libbearpig/regexparser.h>
#include <libbearpig/regexscanner.h>

#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>

int main(int argc, char **argv) {
  // std::string input("((a|abab)*b+)?");
  // std::string input("(a)");
  std::string input(R"(abc)");
  std::string query(R"([abc\[]\[)");
  argparse::ArgumentParser program(argv[0]);
  program.add_argument("query").help("regex to use as a query");
  program.add_argument("input").help("input string to search");
  program.add_argument("-v").flag().help("enable verbose logging");

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &e) {
    spdlog::error(e.what());
    exit(1);
  }

  if (program.is_used("-v")) {
    spdlog::set_level(spdlog::level::debug);
  }

  if (program.is_used("query")) {
    query = program.get<std::string>("query");
  }

  if (program.is_used("input")) {
    input = program.get<std::string>("input");
  }

  RegexScanner scanner{query};
  auto tokens = scanner.tokenize();
  // for (auto token : tokens) {
  //   spdlog::info("token: {} ({}) at {}", token.data,
  //                to_string(token.tokentype), token.column);
  // }
  RegexParser regex_parser{tokens};
  if (regex_parser.parse()) {
    spdlog::debug("successful parse!");
    AlternativeExp *top = regex_parser.get_top_of_expression();
    if (program.is_used("-v")) {
      PrintVisitor p{};
      top->apply(&p);
    }
  } else {
    spdlog::warn("unsuccessful parse");
    spdlog::warn("current_token_idx: {}, tokenstream.size: {}",
                 regex_parser.get_current_token_idx(),
                 regex_parser.get_size_of_tokenstream());
    spdlog::warn("parser is {}", regex_parser.is_done() ? "done" : "not done");
    exit(1);
  }

  NFA nfa;
  NfaGenVisitor nfagen{nfa, tokens};
  AlternativeExp *top = regex_parser.get_top_of_expression();
  top->apply(&nfagen);
  nfa.to_dot();

  spdlog::info("found exact match: {}", nfa.exact_match(input).success);
  auto substringmatch = nfa.find_first_match(input);
  spdlog::info("found sub string match: {}", substringmatch.success);
  spdlog::info("first matched string: {}", substringmatch.match);
  spdlog::info(input);
  std::string padding = fmt::format("{:>{}}", "", substringmatch.start);
  std::string diag_line = fmt::format("{:^>{}}", "^", substringmatch.length);
  spdlog::info("\033[33m{}{}\033[0m", padding, diag_line);

  spdlog::info("trying to find all matches.");
  auto matches = nfa.find_all_matches(input);
  spdlog::info("found {} matches", matches.size());
  for(auto match : matches){
    spdlog::info(" {}", match.match);
  }

  return 0;
}
