#include <fmt/core.h>
#include <fstream>
#include <libbearpig/nfa.h>

void NFA::to_dot(std::filesystem::path dotfile) const {

  std::ofstream outstream{dotfile};

  outstream << "digraph{";
  outstream << "rankdir=LR;";

  for (auto key_val : states) {
    for (auto edge_transition_tuple : key_val.second.transitions) {
      Transition transition = edge_transition_tuple.second;
      outstream << fmt::format("{}->{}[label={}];", transition.from,
                               transition.to, transition.edge);
    }
  }
  outstream << "}";
  return;
}

size_t NFA::add_state() {
  State s;
  s.id = next_index++;
  states.insert({s.id, s});
  return s.id;
}

void NFA::fill_with_dummy_data() {
  size_t index = add_state();
  add_transition_to_state(index, {index, index + 1, 'a'});

  index = add_state();
  add_transition_to_state(index, {index, index + 1, 'b'});

  add_transition_to_state(index, {index, index, 'b'});
  add_transition_to_state(index, {index, index + 2, 's'});

  index = add_state();
  add_transition_to_state(index, {index, index + 1, 'c'});
  add_transition_to_state(index, {index, index + 1, 't'});
}
