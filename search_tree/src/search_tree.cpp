#include <iostream>
#include <queue>
#include <set>

using namespace std;

// Indica el numero de nodos expandido por profundidad.
vector<unsigned> n_visited;
uint64_t n_nodes = 0;
// Promedio del factor de ramificacion.
float prom = 0;

/*
  Imprimimos el resumen del arbol de busqueda.
*/
void print_summary(int u) {
  cout << "\nNUMERO DE NODOS EXPANDIDOS:        " << n_nodes << "\n";
  cout << "FACTOR DE RAMIFICACION PROMEDIO:   " << prom << "\n";
  cout << "NODOS EXPANDIDOS POR PROFUNDIDAD:  \n[";
  for (unsigned depth : n_visited) { cout << depth << ", "; }
  cout << "]\n";
  exit(0);
}

/*
  Para poder parar la ejecucion.
*/
void set_handler(void) {
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = print_summary;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);
}

int main(int argc, char** argv) {
  set_handler();

  bool pruning;

  if (argc != 2) {
    cout << "Invalid syntax.\n";
    return 1;
  }
  if (strcmp(argv[1], "--pruning") == 0) { pruning = true; }
  else if (strcmp(argv[1], "--not-pruning") == 0) { pruning = false; }
  else {
    cout << "Invalid flag.\n";
    return 1;
  }

  // Estados por recorrer. El unsigned indica la profundidad de ese estado.
  queue<pair<unsigned, state_t*>> open;
  pair<unsigned, state_t*> p;
  // Para verificar si un estado ya fue agregado.
  set<uint64_t> visited;
  uint64_t hash_value;

  // Variables necesarias para los sucesores.
  state_t *state;
  ruleid_iterator_t iter;
  int ruleid, d, n_pred;

  // Agregamos los estados objetivos.
  n_visited.push_back(0);
  state = new state_t;
  first_goal_state(state, &d);
  do {
    open.push({0, state});
    if (pruning) { visited.insert(hash_state(state)); }
    state = new state_t;
  } while( next_goal_state(state, &d) );
  delete state;

  while (! open.empty()) {
    p = open.front();
    open.pop();

    // Actualizamos los nodos expandidos.
    n_nodes++;
    if (p.first >= n_visited.size()) { n_visited.push_back(1); }
    else { n_visited[p.first]++; }

    // Miramos todos los predecesores del estado.
    n_pred = 0;
    init_bwd_iter(&iter, p.second);
    while( (ruleid = next_ruleid(&iter) ) >= 0 ) {
      state = new state_t;
      apply_bwd_rule(ruleid, p.second, state);
      n_pred++;

      // Si estamos podando duplicados y ya visitamos el nodo.
      if (pruning) { hash_value = hash_state(state); }
      if (! pruning || (visited.count(hash_value) == 0)) {
        if (pruning) { visited.insert(hash_value); }
        open.push({p.first + 1, state});
      } else { delete state; }
    }
    delete p.second;
    // Actualizamos el promedio del factor de ramificacion
    prom = ((n_nodes - 1) * prom + n_pred) / n_nodes;
  }

  print_summary(0);
  return 0;
}

