#ifndef NFA_H_
#define NFA_H_
#include <filesystem>
#include <map>
#include <set>
#include <vector>

namespace bp {

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

struct RegexMatch {
  bool success;
  size_t start;
  size_t length;
  std::string match;
};

struct NFA {
private:
  friend class NfaGenVisitor;
  std::set<size_t> get_all_available_epsilon_transitions(size_t current);
  std::set<char> get_possible_first_characters();
  size_t next_id = 0;
  RegexMatch run_nfa(std::string_view input, bool exact, size_t start_id = 0);
  State currentAccept;
  std::map<size_t, State> states;
  size_t add_state();
  void add_transition_to_state(size_t state_id, const Transition &transition) {
    states.at(state_id).transitions.insert({transition.edge, transition});
  }
  void add_transition_to_state(size_t state_id, size_t to, char edge) {
    Transition transition{state_id, to, edge};
    states.at(state_id).transitions.insert({edge, transition});
  }

public:
  NFA() {
    State init{{}, 0, true};
    states.insert({0, init});
    currentAccept = init;
  }

  void fill_with_dummy_data();
  void to_dot(std::filesystem::path dotfile =
                  std::filesystem::path("./dot/test.dot")) const;
  RegexMatch exact_match(std::string_view input);
  RegexMatch find_first_match(std::string_view input);
  std::vector<RegexMatch> find_all_matches(std::string_view input);
};

} // namespace bp


#endif // NFA_H_
