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
    "  2: Usando PDBs.\n"
  ;
  if (fgets(str, sizeof(str), stdin) == NULL) {
    cout << "Error: input invalido.\n";
    return 1; 
  }
  opt = atoi(str);
  switch(opt) {
    case 1: h = manhattan; break;

    case 2: 
      cout << "Indique la direccion del directorio donde se encuentran los PDB: ";
      fflush(stdout);
      if (fgets(str, sizeof(str), stdin) == NULL) {
        cout << "Error: input invalido.\n";
        return 1; 
      }
      // Eliminamos el salto de linea.
      str[strlen(str)-1] = '\0';
      init_pdbs(str);

      // Indicar si las heuristicas son aditivas o no.
      cout <<
        "\nIndique si la heuristica es aditiva o no:\n"
        "  1: Si\n"
        "  2: No\n"
      ;
      if (fgets(str, sizeof(str), stdin) == NULL) {
        cout << "Error: input invalido.\n";
        return 1; 
      }
      opt = atoi(str);
      switch(opt) {
        case 1: set_sum(); break;
        case 2: set_max(); break;
        default:
          cout << "Opcion invalida.\n";
          exit(1);
      }

      // Seleccionar el puzzle.
      cout <<
        "\nIndique el puzzle:\n"
        "  1: 15Puzzle\n"
        "  2: 24Puzzle\n"
        "  3: Torres de Hanoi con 12 Discos\n"
        "  4: Torres de Hanoi con 14 Discos\n"
        "  5: Torres de Hanoi con 18 Discos\n"
        "  6: Top Spin de longitud 12\n"
        "  7: Top Spin de longitud 14\n"
        "  8: Top Spin de longitud 17\n"
        "  9: Cubo de Rubik\n"
      ;
      if (fgets(str, sizeof(str), stdin) == NULL) {
        cout << "Error: input invalido.\n";
        return 1; 
      }
      opt = atoi(str);
      switch(opt) {
        case 1: set_15puzzle(); h = pdb_Npuzzle; break;
        case 2: set_24puzzle(); h = pdb_Npuzzle; break;
        case 3: set_hanois12D(); h = pdb_hanois; break;
        case 4: set_hanois14D(); h = pdb_hanois; break;
        case 5: set_hanois18D(); h = pdb_hanois; break;
        case 6: set_topspin12(); h = pdb_topspin; break;
        case 7: set_topspin14(); h = pdb_topspin; break;
        case 8: set_topspin17(); h = pdb_topspin; break;
        case 9: h = pdb_rubik; break;
        default:
          cout << "Opcion invalida.\n";
          exit(1);
      }
      break;
    
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