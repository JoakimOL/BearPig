#ifndef REGEXSCANNER_H_
#define REGEXSCANNER_H_
#include <libbearpig/regextokens.h>
#include <string_view>
#include <vector>

class RegexScanner {
public:
  RegexScanner(const std::string_view input);
  RegexToken next();
  std::vector<RegexToken> tokenize();
  bool is_at_end() const { return current_column == input.size(); }

private:
  const std::string_view input;
  int current_column;
};

#endif // REGEXSCANNER_H_
