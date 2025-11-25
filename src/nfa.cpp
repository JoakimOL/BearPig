#include "spdlog/spdlog.h"
#include <fstream>
#include <fmt/format.h>
#include <libbearpig/nfa.h>
#include <stack>

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

std::set<size_t> NFA::epsilon_things(size_t current){
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

bool NFA::match(const std::string &input) {
  std::set<size_t> current_states{0};
  current_states.merge(epsilon_things(0));
  std::vector<Transition> available_transitions;
  size_t current_input{0};
  char current_char = input[current_input];

  // while (current_input < input.size()) {
  while (true) {

    std::set<size_t> next_states;
    std::set<size_t> epsilon_states;
    spdlog::info("current_char: {}({})", current_char, (int)current_char);
    for (auto state : current_states) {
      spdlog::info(
          "curren_states: {} is_accept={} current_input = {} input_size= {}",
          state, states.at(state).is_accept, current_input, input.size());
      if (states.at(state).is_accept && current_input == input.size()) {
        return true;
      }
    }
    for (auto state : current_states) {
      auto current_state = states.at(state);
      spdlog::info(
          "iterating: looking for {}. found state {} with transitions:",
          current_char, state);
      for (auto transition : current_state.transitions) {
        spdlog::info("from: {} to: {} edge: {}", transition.second.from,
                     transition.second.to, transition.second.edge);
      }
      if (current_input < input.size() &&
          current_state.transitions.contains(current_char)) {
        spdlog::info("state {} has a transition matching the character {}!",
                     state, current_char);
        next_states.insert(
            current_state.transitions.find(current_char)->second.to);
      }
      // while (next_states.transitions.contains(0)) {
      //   spdlog::info("state {} has an epsilon transition!", state);
      //   next_states.insert(
      //       current_state.transitions.extract(0).mapped().to);
      // }
    }

    // std::set<size_t> epsilon_states;
    // for(auto state: next_states){
    //   while(states.at(state).transitions.contains(0)){
    //     epsilon_states.insert(states.at(state).transitions.extract(0).mapped().to);
    //   }
    // }
    // p
    for(auto state : next_states){
      epsilon_states.merge(epsilon_things(state));
    }
    next_states.merge(epsilon_states);
    current_input++;

    if (next_states.size() == 0) {
      spdlog::error("no next states, fuck off");
      return false;
    }
    current_states = next_states;
    // current_states.merge(epsilon_states);
    next_states.clear();
    current_char = input[current_input];
  }
  return false;
}
