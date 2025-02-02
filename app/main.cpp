#include <libbearpig/lib.h>
#include <libbearpig/nfa.h>
int main() {
  int res = foo();
  NFA nfa;
  nfa.fill_with_dummy_data();
  nfa.to_dot();
  return res;
}
