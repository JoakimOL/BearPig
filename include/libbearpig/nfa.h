#ifndef NFA_H_
#define NFA_H_
#include <filesystem>
#include <map>

struct Transition {
  size_t from; // redundant information?
  size_t to;
  char edge;
};

struct State {
  std::multimap<char, Transition> transitions;
  size_t id;
  bool is_accept;
};

struct NFA {
private:
  size_t next_id = 0;

public:
  NFA() {
    State init{{}, 0, true};
    states.insert({0, init});
    currentAccept = init;
  }
  std::map<size_t, State> states;
  State currentAccept;
  void fill_with_dummy_data();
  void to_dot(std::filesystem::path dotfile =
                  std::filesystem::path("./dot/test.dot")) const;
  size_t add_state();

  void add_transition_to_state(size_t state_id, const Transition &transition) {
    states.at(state_id).transitions.insert({transition.edge, transition});
  }
  void add_transition_to_state(size_t state_id, size_t to, char edge) {
    Transition transition{state_id, to, edge};
    states.at(state_id).transitions.insert({edge, transition});
  }
};

#endif // NFA_H_
