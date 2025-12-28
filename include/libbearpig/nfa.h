#ifndef NFA_H_
#define NFA_H_
#include <filesystem>
#include <map>
#include <set>
#include "fmt/core.h"
#include "fmt/ranges.h"

struct Transition {
  size_t from; // redundant information?
  size_t to;
  char edge;
};

template <> struct fmt::formatter<Transition> {
    constexpr auto parse(format_parse_context &ctx) { return ctx.end(); }
    template <typename FormatContext>
    auto format(const Transition & trans, FormatContext& ctx) const {
      return fmt::format_to(ctx.out(), "(from {} to {} {})", trans.from, trans.to,
                            trans.edge == 0 ? ""
                                         : fmt::format("over {}", trans.edge));
    }
};

struct State {
  std::multimap<char, Transition> transitions;
  size_t id;
  bool is_accept;
};

template <> struct fmt::formatter<State> {
    constexpr auto parse(format_parse_context &ctx) { return ctx.end(); }
    template <typename FormatContext>
    auto format(const State & state, FormatContext& ctx) const {
      return fmt::format_to(ctx.out(), "{}:{}", state.id, state.transitions);
    }
};

struct RegexMatch{
  bool success;
  size_t start;
  size_t length;
  std::string match;
};

struct NFA {
private:
  std::set<size_t> get_all_available_epsilon_transitions(size_t current);
  size_t next_id = 0;
  RegexMatch run_nfa(std::string_view input, bool exact, size_t start_id = 0);

public:
  NFA() {
    State init{{}, 0, true};
    states.insert({0, init});
    currentAccept = init;
  }
  State currentAccept;
  std::map<size_t, State> states;

  void fill_with_dummy_data();
  void to_dot(std::filesystem::path dotfile =
                  std::filesystem::path("./dot/test.dot")) const;
  size_t add_state();
  RegexMatch match(std::string_view input);
  RegexMatch find_first(std::string_view input);

  void add_transition_to_state(size_t state_id, const Transition &transition) {
    states.at(state_id).transitions.insert({transition.edge, transition});
  }
  void add_transition_to_state(size_t state_id, size_t to, char edge) {
    Transition transition{state_id, to, edge};
    states.at(state_id).transitions.insert({edge, transition});
  }
};

#endif // NFA_H_
