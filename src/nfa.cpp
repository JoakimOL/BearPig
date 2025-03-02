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
  State state;
  state.id = next_id++;
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
