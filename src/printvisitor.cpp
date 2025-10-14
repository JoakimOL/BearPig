#include "spdlog/spdlog.h"
#include <cstdarg>
#include <libbearpig/printvisitor.h>
#include <libbearpig/regexast.h>

void PrintVisitor::visit(AlternativeExp &exp) {

  std::string indentation = fmt::format("{: >{}}", "", depth);
  spdlog::info("{}{}:num_alternatives: {} (depth: {})", indentation,
               "AlternativeExp", exp.alternatives.size(), depth);
  return;
}
void PrintVisitor::visit(ConcatExp &exp) {
  std::string indentation = fmt::format("{: >{}}", "", depth);
  spdlog::info("{}{}:num concatenations: {} (depth: {})", indentation,
               "ConcatExp", exp.exps.size(), depth);
  return;
}
void PrintVisitor::visit(QuantifiedExp &exp) {
  std::string indentation = fmt::format("{: >{}}", "", depth);
  spdlog::info("{}{}:quantifier: {} (depth: {})", indentation, "QuantifiedExp",
               exp.to_string(exp.quantifier), depth);
  return;
}
void PrintVisitor::visit(GroupExp &exp) {
  std::string indentation = fmt::format("{: >{}}", "", depth);
  spdlog::info("{}{} (depth: {})", indentation, "GroupExp", depth);
  return;
}
void PrintVisitor::visit(SetExp &exp) {
  std::string indentation = fmt::format("{: >{}}", "", depth);
  spdlog::info("{} {} {} (depth: {})", indentation, "SetExp",
               exp.negative ? "(Negative)" : "", depth);
  return;
}
void PrintVisitor::visit(SetItem &exp) {
  std::string indentation = fmt::format("{: >{}}", "", depth);
  std::string start = exp.start.to_string();
  std::string stop = exp.range ? exp.stop.to_string() : "";
  spdlog::info("{}{} start: {} range: {} stop: {} (depth: {})", indentation,
               "SetItem", start, exp.range ? "yes" : "no", stop, depth);
  return;
}
void PrintVisitor::visit(RChar &exp) {
  std::string indentation = fmt::format("{: >{}}", "", depth);
  spdlog::info("{}{}:{} (depth: {})", indentation, "RChar", exp.to_string(),
               depth);
  return;
}
void PrintVisitor::visit(AnyExp &exp) {
  std::string indentation = fmt::format("{: >{}}", "", depth);
  spdlog::info("{}{} (depth: {})", indentation, "Any", depth);
  return;
}
void PrintVisitor::visit(EscapeSeq &exp) {
  std::string indentation = fmt::format("{: >{}}", "", depth);
  spdlog::info("{}{}: {} (depth: {})", indentation, "Escape", exp.character.data, depth);
  return;
}
