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
  cout << "Introduzca el \e[1;3mestado\e[0m inicial: ";
  if (fgets(str, sizeof(str), stdin) == NULL) {
    cout << "Error: estado vacio.\n";
    return 1; 
  }

  // Convert the string to a state
  nchars = read_state(str, s_init);
  if (nchars <= 0) {
    cout << "Error: estado invalido.\n";
    return 2; 
  }

  // Select the heuristic.
  cout <<
    "\nIndique cual \e[1;3mheuristica\e[0m quiere usar:\n"
    "  1: Manhattan (15Puzzle).\n"
    "  2: Additive PDB (15Puzzle).\n"
    "  3: Additive PDB (24Puzzle).\n"
    "  4: Additive PDB (Towers of Hanoi 4P 12D).\n"
  ;
  if (fgets(str, sizeof(str), stdin) == NULL) {
    cout << "Error: input invalido.\n";
    return 1; 
  }
  opt = atoi(str);
  if (opt != 1) {
    cout << "Indique la direccion del directorio donde se encuentran los PDB: ";
    fflush(stdout);
    if (fgets(str, sizeof(str), stdin) == NULL) {
      cout << "Error: input invalido.\n";
      return 1; 
    }
    // Eliminamos el salto de linea.
    str[strlen(str)-1] = '\0';
    init_pdbs(str);
  }
  switch(opt) {
    case 1: h = manhattan; break;
    case 2: h = additive_pdb_15puzzle; break;
    case 3: h = additive_pdb_24puzzle; break;
    case 4: h = additive_pdb_hanoi12D; break;
    default:
      cout << "Opcion invalida.\n";
      exit(1);
  }

  // Select the informed search function.
  cout << 
    "\nIndique cual \e[1;3mbusqueda\e[0m quiere usar:\n"
    "  1: A*.\n"
    "  2: A* con eliminacion de duplicados.\n"
    "  3: A* con eliminacion tardia de duplicados.\n"
    "  4: IDA*.\n"
    "  5: IDA* con eliminacion de duplicados.\n"
    "  6: IDA* con eliminacion parcial de duplicados.\n"
  ;
  if (fgets(str, sizeof(str), stdin) == NULL) {
    cout << "Error: input invalido.\n";
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
        cout << "Error: input invalido.\n";
        exit(1); 
    }

    // Imprimimos la solucion.
    if (solution != NULL) {
      vector<int> rules = solution->extract_path();
      cout << "Solution size: " << rules.size() << "\n";
    }

  } else {

    vector<int> rules;
    switch(opt) {
      case 4: rules = ida(s_init, h, false); break;
      case 6: rules = ida(s_init, h, true); break;
      default:
        cout << "Error: input invalido.\n";
        exit(1); 
    }

    // Imprimimos la longitud de la solucion.
    if (! rules.empty()) {
      cout << "Solution size: " << rules.size() << "\n";
    }
  } 

  return 0;
}