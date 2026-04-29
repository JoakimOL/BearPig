#include "spdlog/spdlog.h"
#include <libbearpig/printvisitor.h>
#include <libbearpig/regexast.h>

namespace bp {

void PrintVisitor::operator()(this PrintVisitor &self, AlternativeExp &exp) {

  std::string indentation = fmt::format("{: >{}}", "", self.depth);
  spdlog::info("{}{}:num_alternatives: {} (depth: {})", indentation,
               "AlternativeExp", exp.alternatives.size(), self.depth);
  self.depth++;
  for_each(exp.alternatives.begin(), exp.alternatives.end(),
           [&self](ConcatExp &c) { self(c); });
  self.depth--;
  return;
}
void PrintVisitor::operator()(this PrintVisitor &self, ConcatExp &exp) {
  std::string indentation = fmt::format("{: >{}}", "", self.depth);
  spdlog::info("{}{}:num concatenations: {} (depth: {})", indentation,
               "ConcatExp", exp.exps.size(), self.depth);
  self.depth++;
  for_each(exp.exps.begin(), exp.exps.end(),
           [&self](QuantifiedExp &quantexp) { self(quantexp); });
  self.depth--;
  return;
}
void PrintVisitor::operator()(this PrintVisitor &self, QuantifiedExp &exp) {
  std::string indentation = fmt::format("{: >{}}", "", self.depth);
  spdlog::info("{}{}:quantifier: {} (depth: {})", indentation, "QuantifiedExp",
               exp.to_string(exp.quantifier), self.depth);
  self.depth++;
  std::visit(self, exp.exp);
  self.depth--;
  return;
}
void PrintVisitor::operator()(this PrintVisitor &self, GroupExp &exp) {
  std::string indentation = fmt::format("{: >{}}", "", self.depth);
  spdlog::info("{}{} (depth: {})", indentation, "GroupExp", self.depth);
  self.depth++;
  // exp.subExp->apply(this);
  self(*exp.subExp);
  self.depth--;
  return;
}
void PrintVisitor::operator()(this PrintVisitor &self, SetExp &exp) {
  std::string indentation = fmt::format("{: >{}}", "", self.depth);
  spdlog::info("{} {} {} (depth: {})", indentation, "SetExp",
               exp.negative ? "(Negative)" : "", self.depth);
  self.depth++;
  for_each(exp.items.begin(), exp.items.end(),
           [&self](auto &item) { self(item); });
  self.depth--;
  return;
}
void PrintVisitor::operator()(this PrintVisitor &self, SetItem &exp) {
  std::string indentation = fmt::format("{: >{}}", "", self.depth);
  std::string start = exp.start.to_string();
  std::string stop = exp.range ? exp.stop.to_string() : "";
  spdlog::info("{}{} start: {} range: {} stop: {} (depth: {})", indentation,
               "SetItem", start, exp.range ? "yes" : "no", stop, self.depth);
  return;
}
void PrintVisitor::operator()(this PrintVisitor &self, RChar &exp) {
  std::string indentation = fmt::format("{: >{}}", "", self.depth);
  spdlog::info("{}{}:{} (depth: {})", indentation, exp.is_escape ? "Escape": "RChar", exp.to_string(),
               self.depth);
  return;
}
void PrintVisitor::operator()(this PrintVisitor &self, AnyExp &exp) {
  std::string indentation = fmt::format("{: >{}}", "", self.depth);
  spdlog::info("{}{} (depth: {})", indentation, "Any", self.depth);
  return;
}
} // namespace bp
