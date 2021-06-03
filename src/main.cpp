#include "head.hpp"

using namespace std;

int main(void) {
  // VARIABLES FOR INPUT
  char str[MAX_LINE_LENGTH + 1];
  ssize_t nchars; 
  state_t *s_init = new state_t; 

  // READ A LINE OF INPUT FROM stdin
  printf("Introduzca el estado inicial: ");
  if (fgets(str, sizeof str, stdin) == NULL) {
    printf("Error: estado vacio.\n");
    return 1; 
  }

  // CONVERT THE STRING TO A STATE
  nchars = read_state(str, s_init);
  if (nchars <= 0) {
    printf("Error: estado invalid.\n");
    return 2; 
  }

  //Node* solution = ida_star(s_init, h);
  Node* solution = best_first_search(s_init, f);
  if (solution == NULL) {
    cout << "Not Solution.\n";
    return 1;
  } else {
    cout << "There is a solution!\n";
  }
  cout << "\n";
  vector<int> rules = solution->extract_path();
  for (vector<int>::reverse_iterator it = rules.rbegin(); it != rules.rend(); it++) {
    cout << get_fwd_rule_label(*it) << "\n";
  }
  cout << "\n";

  return 0;
}

/* Valor absoluto. */
int abs(int x) { 
  if (x < 0) return -x;
  return x;
}

/* Heuristica. */
int h(state_t *s) { return 0; }

// Funcion de evaluacion.
int f(Node *node) { return node->g + h(node->state); }