#include "spdlog/spdlog.h"
#include <fmt/format.h>
#include <fstream>
#include <libbearpig/nfa.h>
#include <stack>
#include <vector>
#include "fmt/core.h"
#include "fmt/ranges.h"

namespace bp {

void NFA::to_dot(std::filesystem::path dotfile) const {

  std::ofstream outstream{dotfile};

  outstream << "digraph{";
  outstream << "rankdir=LR;";
  outstream << "node[shape=circle];";

  for (auto key_val : states) {
    for (auto edge_transition_tuple : key_val.second.transitions) {
      Transition transition = edge_transition_tuple.second;
      bool accept = currentAccept.id == transition.to;
      if (accept) {
        outstream << fmt::format("{}[shape=doublecircle]", transition.to);
      }
      outstream << fmt::format(
          "{}->{}[label={}];", transition.from, transition.to,
          transition.edge == 0
              ? "\"\""
              : fmt::format("\"{}{}\"", transition.edge == '\\' ? "\\" : "",
                            std::string{transition.edge}));
    }
  }
  outstream << "}";
  return;
}

size_t NFA::add_state() {
  State state{{}, ++next_id, false};
  states.insert({state.id, state});
  return state.id;
}

void NFA::fill_with_dummy_data() {
  size_t state_id = add_state();
  add_transition_to_state(state_id, state_id + 1, 'a');

  state_id = add_state();
  add_transition_to_state(state_id, state_id + 1, 'b');

  add_transition_to_state(state_id, state_id, 'b');
  add_transition_to_state(state_id, state_id + 2, 's');

  state_id = add_state();
  add_transition_to_state(state_id, state_id + 1, 'c');
  add_transition_to_state(state_id, state_id + 1, 't');
}

std::set<size_t> NFA::get_all_available_epsilon_transitions(size_t current) {
  std::set<size_t> epsilon_states;
  std::stack<size_t> stack;
  stack.push(current);
  while (!stack.empty()) {
    size_t current = stack.top();
    stack.pop();

    if (epsilon_states.find(current) != epsilon_states.end()) {
      continue;
    }
    epsilon_states.insert(current);
    const auto &current_state = states.at(current);
    for (const auto &transition : current_state.transitions) {
      if (transition.first == 0) {
        stack.push(transition.second.to);
      }
    }
  }
  return epsilon_states;
}

std::set<char> NFA::get_possible_first_characters() {
  std::set<char> starts{};
  auto init = get_all_available_epsilon_transitions(0);
  for (size_t id : init) {
    auto state = states.at(id);
    for (auto transition : state.transitions) {
      if (transition.first != 0) {
        starts.insert(transition.first);
      }
    }
  }
  return starts;
}

std::vector<RegexMatch> NFA::find_all_matches(std::string_view input) {
  std::set<char> starts = get_possible_first_characters();
  std::vector<RegexMatch> matches{};
  size_t i = 0;
  while (i <= input.size()) {
    for (; i < input.size() && !starts.contains(input[i]); i++)
      ;
    auto match = run_nfa(input.substr(i), false, i);
    if (match.success) {
      matches.emplace_back(match);
      i += match.length;
    } else {
      i++;
    }
  }
  return matches;
}

RegexMatch NFA::find_first_match(std::string_view input) {
  std::set<char> starts = get_possible_first_characters();
  RegexMatch match{.success = false};
  size_t i = 0;
  while (i <= input.size() && !match.success) {
    for (; i < input.size() && !starts.contains(input[i]); i++)
      ;
    match = run_nfa(input.substr(i), false, i);
    i++;
  }
  return match;
}

RegexMatch NFA::exact_match(std::string_view input) {
  return run_nfa(input, true);
}

RegexMatch NFA::run_nfa(std::string_view input, bool exact, size_t start_id) {
  RegexMatch result{.success = false, .start = start_id};
  std::set<size_t> current_states{0};
  current_states.merge(get_all_available_epsilon_transitions(0));
  std::vector<Transition> available_transitions;
  size_t current_input{0};
  char current_char = input[current_input];
  bool should_greed{false};

  while (true) {

    std::set<size_t> next_states;
    std::set<size_t> epsilon_states;
    spdlog::debug("current_char: {}({})", current_char, (int)current_char);
    for (auto state : current_states) {
      spdlog::debug(
          "curren_states: {} is_accept={} current_input = {} input_size= {}",
          state, currentAccept.id == state, current_input, input.size());
      if (currentAccept.id == state &&
          (current_input == input.size() || !exact)) {
        result.length = current_input;
        result.success = true;
        result.match = std::string{input.substr(0, current_input)};
        if (!should_greed) {
          return result;
        }
      }
    }
    should_greed = false;
    for (auto state : current_states) {
      auto current_state = states.at(state);
      spdlog::debug(
          "iterating: looking for {}. found state {} with transitions:",
          current_char, state);
      for (auto transition : current_state.transitions) {
        spdlog::debug("from: {} to: {} edge: {}", transition.second.from,
                      transition.second.to, transition.second.edge);
      }
      if (current_input < input.size() &&
          current_state.transitions.contains(current_char)) {
        spdlog::debug("state {} has a transition matching the character {}!",
                      state, current_char);
        next_states.insert(
            current_state.transitions.find(current_char)->second.to);
        should_greed = true;
      }
    }
    for (auto state : next_states) {
      epsilon_states.merge(get_all_available_epsilon_transitions(state));
    }
    next_states.merge(epsilon_states);
    current_input++;

    if (next_states.size() == 0) {
      return result;
    }
    current_states = next_states;
    next_states.clear();
    current_char = input[current_input];
  }
  return result;
}
} // namespace bp

template <> struct fmt::formatter<bp::Transition> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.end(); }
  template <typename FormatContext>
  auto format(const bp::Transition &trans, FormatContext &ctx) const {
    return fmt::format_to(ctx.out(), "(from {} to {} {})", trans.from, trans.to,
                          trans.edge == 0 ? ""
                                          : fmt::format("over {}", trans.edge));
  }
};

template <> struct fmt::formatter<bp::State> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.end(); }
  template <typename FormatContext>
  auto format(const bp::State &state, FormatContext &ctx) const {
    return fmt::format_to(ctx.out(), "{}:{}", state.id, state.transitions);
  }
};

