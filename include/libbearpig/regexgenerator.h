#ifndef REGEX_GENERATOR_H__
#define REGEX_GENERATOR_H__

#include "libbearpig/nfa.h"
#include "libbearpig/regexparser.h"
#include "libbearpig/regexscanner.h"
#include <string_view>

namespace bp {

struct Regex {
  std::unique_ptr<NFA> nfa;
  std::string_view input;
  RegexMatch find_first_match(std::string_view input){
    return nfa->find_first_match(input);
  }
  RegexMatch exact_match(std::string_view input){
    return nfa->exact_match(input);
  }
  std::vector<RegexMatch> find_all_matches(std::string_view input){
    return nfa->find_all_matches(input);
  }
};

class RegexGenerator {
  public:
    explicit RegexGenerator();
    explicit RegexGenerator(std::string_view input);
    bool compile_regex(std::string_view input);
  private:
    RegexScanner scanner;
    RegexParser parser;
};

} // namespace bp

#endif // REGEX_GENERATOR_H__
