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
  printf("Introduzca el \e[1;3mestado\e[0m inicial: ");
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
    "\nIndique cual \e[1;3mheuristica\e[0m quiere usar:\n"
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
    "\nIndique cual \e[1;3mbusqueda\e[0m quiere usar:\n"
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
  cout << "\n\n";
  opt = atoi(str);
  if (opt < 6 && opt != 4) {
    
    Node* solution;
    switch(opt) {
      case 1: solution = best_first_search(s_init, f); break;
      case 2: solution = best_first_search_dup_pruning(s_init, f); break;
      case 3: solution = best_first_search_late_dup_pruning(s_init, f); break;
      case 5: solution = ida_dup_pruning(s_init, h); break;
      default:
        printf("Error: input invalido.\n");
        exit(1); 
    }

    // Imprimimos la solucion.
    if (solution != NULL) {
      cout << "Solution: [";
      vector<int> rules = solution->extract_path();
      for (vector<int>::reverse_iterator it = rules.rbegin(); it != rules.rend(); it++) {
        cout << get_fwd_rule_label(*it) << ", ";
      }
      cout << "]\n";
    }

  } else {

    vector<int> rules;
    switch(opt) {
      case 4: rules = ida(s_init, h, false); break;
      case 6: rules = ida(s_init, h, true); break;
      default:
        printf("Error: input invalido.\n");
        exit(1); 
    }

    // Imprimimos la longitud de la solucion.
    if (! rules.empty()) {
      cout << "Solution size: " << rules.size() << "\n";
    }
  } 

  return 0;
}