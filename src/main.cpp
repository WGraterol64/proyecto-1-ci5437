#include "head.hpp"

// Variable global que almacenara una heuristica.
unsigned (*h) (state_t*);

// Funcion de evaluacion de nodos.
unsigned f(Node *node) { return node->g + h(node->state); }

int main(void) {
  // Variables for input.
  char str[MAX_LINE_LENGTH + 1];
  ssize_t nchars; 
  state_t *s_init = new state_t; 
  int opt;

  // Read a line from stdin
  printf("Introduzca el estado inicial: ");
  if (fgets(str, sizeof(str), stdin) == NULL) {
    printf("Error: estado vacio.\n");
    return 1; 
  }

  // Convert the string to a state
  nchars = read_state(str, s_init);
  if (nchars <= 0) {
    printf("Error: estado invalido.\n");
    return 2; 
  }

  // Select the heuristic.
  printf(
    "Indique cual heuristica quiere usar:\n"
    "  1: Manhattan (Sliding Tile).\n"
  );
  if (fgets(str, sizeof(str), stdin) == NULL) {
    printf("Error: input invalido.\n");
    return 1; 
  }
  opt = atoi(str);
  switch(opt) {
    case 1: 
      h = manhattan;
      break;
    default:
      printf("Opcion invalida.\n");
      exit(1);
  }

  // Select the informed search function.
  printf(
    "Indique cual funcion quiere usar:\n"
    "  1: A*.\n"
    "  2: A* con eliminacion de duplicados.\n"
    "  3: A* con eliminacion tardia de duplicados.\n"
    "  4: IDA*.\n"
    "  5: IDA* con eliminacion de duplicados.\n"
    "  6: IDA* con eliminacion parcial de duplicados.\n"
  );
  if (fgets(str, sizeof(str), stdin) == NULL) {
    printf("Error: input invalido.\n");
    exit(1); 
  }
  opt = atoi(str);
  if (opt < 6) {
    
    Node* solution;
    switch(opt) {
      case 1: solution = best_first_search(s_init, f); break;
      case 2: solution = best_first_search_dup_pruning(s_init, f); break;
      case 3: solution = best_first_search_late_dup_pruning(s_init, f); break;
      case 4:
      case 5: solution = ida_dup_pruning(s_init, h); break;
      default:
        printf("Error: input invalido.\n");
        exit(1); 
    }

    if (solution != NULL) {
      vector<int> rules = solution->extract_path();
      for (vector<int>::reverse_iterator it = rules.rbegin(); it != rules.rend(); it++) {
        cout << get_fwd_rule_label(*it) << "\n";
      }
    }

  } else if (opt == 6) {
    vector<int> rules = ida_part_dup_pruning(s_init, h);
    for (vector<int>::iterator it = rules.begin(); it != rules.end(); it++) {
      cout << get_fwd_rule_label(*it) << "\n";
    }
  } else {
    printf("Error: input invalido.\n");
    exit(1); 
  }

  return 0;
}