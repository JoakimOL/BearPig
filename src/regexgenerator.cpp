#include "libbearpig/regexscanner.h"
#include <libbearpig/regexgenerator.h>

bool bp::RegexGenerator::compile_regex(std::string_view input) {
  scanner = RegexScanner(input);
  auto tokens = scanner.tokenize();
  parser = RegexParser(tokens);
  auto ast = parser.parse();
  return true;
}
