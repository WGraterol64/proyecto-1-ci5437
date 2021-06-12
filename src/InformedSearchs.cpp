#include "InformedSearchs.hpp"

/*  ======================= VARIABLES GLOBALES ======================= */
// Almacena los nodos visitados por profundidad.
vector<unsigned> *visited = new vector<unsigned>;
// Estado global usado en la funcion IDA* con eliminacion parcial de duplicados.
state_t *state;
// Camino de acciones necesarios para pasar del estado inicial al estado global.
vector<int> path;
// Usada para medir el tiempo de ejecucion
clock_t start;



/*  ======================= FUNCIONES AUXILIARES ======================= */
/* 
  Imprime al vector visited siguiendo la sintaxis de Python para luego hacer
  graficas mas facilmente.
*/
void print_visited(void) {
  // Imprimimos los nodos visitados por profundidad.
  cout <<"Expanded nodes by depth: [";
  for (vector<unsigned>::iterator it = visited->begin(); it != visited->end(); it++) {
    cout << *it << ", ";
  }
  cout <<"]\n";
}

/*
  Imprime la memoria virtual (ram y swap) usada actualmente.
*/
void print_memory_used(void) {
  struct sysinfo memInfo;
  sysinfo (&memInfo);
  double virtualMemUsed = memInfo.totalram - memInfo.freeram;
  //Add other values in next statement to avoid int overflow on right hand side...
  virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
  virtualMemUsed *= memInfo.mem_unit;
  cout << virtualMemUsed / (1024*1024*1024) << " Gb\n";
}

/*
  Imprime el tiempo de ejecucion segun la variables globales start y end.
*/
void print_time(void) {
  // Calculating total time taken by the program.
  double time_taken = double(clock() - start) / double(CLOCKS_PER_SEC);
  cout << "Time: " << fixed << time_taken << setprecision(5);
  cout << " seconds\n";
}

/*
  Imprime los resultados actuales y finaliza la ejecucion.
*/
void handler_ctrl_c(int s){
  // Imprimimos la cantidad de nodos expandidos por profundidad y la 
  // memoria virtual usada.
  cout << "Not solution found.\n";
  print_time();
  cout << "Final memory: ";
  print_memory_used();
  print_visited();
  exit(1); 
}

/*
  Para poder parar la ejecucion.
*/
void set_handler(void) {
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = handler_ctrl_c;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);
}

/*
  Aplica una regla forward sobre un estado y almacena el resultado en el 
  mismo estado.
*/
void apply_rule(int ruleid, state_t *state) {
  // Resulta que no podemos ejecutar:   apply_fwd_rule(ruleid, state, state)
  // porque state se modifica mientras se lee (que implementacion tan chimba)
  // por lo tanto, debemos usar una copia auxiliar para que no hayan errores.
  // Este peo me amargo la vida como por 2 horas.
  state_t state_aux;
  copy_state(&state_aux, state);
  apply_fwd_rule(ruleid, &state_aux, state);
}

/*
  Aplica una regla backward sobre un estado y almacena el resultado en el 
  mismo estado.
*/
void revert_rule(int ruleid, state_t *state) {
  state_t state_aux;
  copy_state(&state_aux, state);
  apply_bwd_rule(ruleid, &state_aux, state);
}



/*  ======================= FUNCIONES PRINCIPALES ======================= */
/*
  Busqueda de mejor camino.

  INPUTS:
    state_t *s_init             =>  Estado inicial de la busqueda.
    unsigned (*f) (Node*)       =>  Funcion de evaluacion de un nodo. Si f es el costo del 
        camino parcial, entonces el algoritmo es UCS; si f es una heuristica, entonces 
        el algoritmo es de busqueda voraz; y si es el costo + heuristica, entonces es A*
  OUTPUT:
    Node* =>  Nodo que contiene la solucion. NULL si no hay solucion.
*/
Node *best_first_search(
    state_t *s_init, 
    unsigned (*f) (Node*)
) {
  // Iniciamos el conteo del tiempo.
  start = clock();

  // En q almacenaremos los nodos que representan los estados a explorar
  // ordenados segun f. Primero agregamos el estado inicial.
  PriorityQueue<unsigned, Node*> q = PriorityQueue<unsigned, Node*>(f);
  Node* node = new Node(s_init);
  q.add(node);

  // Variables para iterar a traves de los sucesores de un determinado estado.
  state_t *succ;
  ruleid_iterator_t iter;
  int ruleid;

  while (true) {
    // Si q se queda vacio, no hay solucion.
    if (q.empty()) {
      // Imprimimos la cantidad de nodos expandidos por profundidad y la 
      // memoria virtual usada.
      print_time();
      cout << "Final memory: ";
      print_memory_used();
      print_visited();
      cout << "No hay solucion.\n";
      return NULL;
    }

    // Obtenemos el siguiente nodo.
    node = q.pop();

    // Actualizamos el conteo de nodos.
    if (visited->size() <= node->d) visited->push_back(1);
    else (*visited)[node->d]++;

    // Verificamos si es solucion.
    if (is_goal(node->state)) {
      // Imprimimos la cantidad de nodos expandidos por profundidad y la 
      // memoria virtual usada.
      print_time();
      cout << "Final memory: ";
      print_memory_used();
      print_visited();
      return node;
    }

    // Inicializamos el iterador de sucesores.
    init_fwd_iter(&iter, node->state);
    while((ruleid = next_ruleid(&iter)) >= 0) {

      // Obtenemos el siguiente sucesor.
      succ = new state_t;
      apply_fwd_rule(ruleid, node->state, succ);
      q.add(node->make_node(succ, ruleid));
    }
  }
}



/*
  Busqueda de mejor camino con eliminacion de duplicados.

  INPUTS:
    state_t *s_init             =>  Estado inicial de la busqueda.
    unsigned (*f) (Node*)       =>  Funcion de evaluacion de un nodo. Si f es el costo del 
        camino parcial, entonces el algoritmo es UCS; si f es una heuristica, entonces 
        el algoritmo es de busqueda voraz; y si es el costo + heuristica, entonces es A*
  OUTPUT:
    Node* =>  Nodo que contiene la solucion. NULL si no hay solucion.
*/
Node *best_first_search_dup_pruning(
    state_t *s_init, 
    unsigned (*f) (Node*)
) {
  // Iniciamos el conteo del tiempo.
  start = clock();

  // En frontier almacenaremos los nodos que representan los estados a explorar
  // ordenados segun f. Primero agregamos el estado inicial.
  NodesPriorityQueue frontier = NodesPriorityQueue(f);
  Node* node = new Node(s_init);
  frontier.add(node);

  // En explored almacenaremos los nodos visitados
  set<uint64_t> explored;

  // Variables para iterar a traves de los sucesores de un determinado estado.
  Node* child;
  state_t *succ;
  ruleid_iterator_t iter;
  int ruleid;

  while (true) {

    // Si frontier se queda vacio, no hay solucion.
    if (frontier.empty()) {
      // Imprimimos la cantidad de nodos expandidos por profundidad y la 
      // memoria virtual usada.
      print_time();
      cout << "Final memory: ";
      print_memory_used();
      print_visited();
      cout << "No hay solucion.\n";
      return NULL;
    }

    // Obtenemos el siguiente nodo.
    node = frontier.pop();

    // Actualizamos el conteo de nodos.
    if (visited->size() <= node->d) visited->push_back(1);
    else (*visited)[node->d]++;

    // Verificamos si es solucion.
    if (is_goal(node->state)) {
      // Imprimimos la cantidad de nodos expandidos por profundidad y la 
      // memoria virtual usada.
      print_time();
      cout << "Final memory: ";
      print_memory_used();
      print_visited();
      return node;
    }
    explored.insert(hash_state(node->state));

    // Inicializamos el iterador de sucesores.
    init_fwd_iter(&iter, node->state);
    while((ruleid = next_ruleid(&iter)) >= 0) {

      // Obtenemos el siguiente sucesor.
      succ = new state_t;
      apply_fwd_rule(ruleid, node->state, succ);
      // Creamos el nodo hijo.
      child = node->make_node(succ, ruleid);

      // Si no lo hemos visto lo agregamos al frontier.
      if (explored.count(hash_state(child->state)) == 0 && ! frontier.find(child)) {
        frontier.add(child);
      // En cambio, si es encuentra en el frontier con un costo mayor, lo sustituimos.
      } else if (frontier.find(child)) {
        frontier.replace_if_less(child);
      }
    }
  }
}



/*
  Busqueda de mejor camino con eliminacion tardia de duplicados.

  INPUTS:
    state_t *s_init             =>  Estado inicial de la busqueda.
    unsigned (*f) (Node*)       =>  Funcion de evaluacion de un nodo. Si f es el costo 
        del camino parcial, entonces el algoritmo es UCS; si f es una heuristica, 
        entonces el algoritmo es de busqueda voraz; y si es el costo + heuristica, 
        entonces es A*
  OUTPUT:
    Node* =>  Nodo que contiene la solucion. NULL si no hay solucion.
*/
Node *best_first_search_late_dup_pruning(
    state_t *s_init, 
    unsigned (*f) (Node*)
) {
  // Iniciamos el conteo del tiempo.
  start = clock();

  // En q almacenaremos los nodos que representan los estados a explorar
  // ordenados segun f. Primero agregamos el estado inicial.
  PriorityQueue<unsigned, Node*> q = PriorityQueue<unsigned, Node*>(f);
  Node* node = new Node(s_init);
  q.add(node);

  // En explored almacenaremos los estados visitados y su costo parcial.
  map<uint64_t, unsigned> explored;
  
  // Variables para iterar a traves de los sucesores de un determinado estado.
  state_t *succ;
  Node *child;
  ruleid_iterator_t iter;
  uint64_t hash_value;
  int ruleid;

  while (! q.empty()) {
    node = q.pop();
    hash_value = hash_state(node->state);

    // Actualizamos el conteo de nodos.
    if (visited->size() <= node->d) visited->push_back(1);
    else (*visited)[node->d]++;

    // Si el estado actual no ha sido visitado o su costo parcial es menor que el 
    // almacenado en explored.
    if (explored.count(hash_value) == 0 || node->g < explored[hash_value]) {

      if (is_goal(node->state)) {
        // Imprimimos la cantidad de nodos expandidos por profundidad y la 
        // memoria virtual usada.
        print_time();
        cout << "Final memory: ";
        print_memory_used();
        print_visited();
        return node;
      }

      explored[hash_value] = node->g;

      // Inicializamos el iterador de sucesores.
      init_fwd_iter(&iter, node->state);
      while((ruleid = next_ruleid(&iter)) >= 0) {

        // Obtenemos el siguiente sucesor y creamos el nodo hijo.
        succ = new state_t;
        apply_fwd_rule(ruleid, node->state, succ);
        child = node->make_node(succ, ruleid);

        // Si el costo del nodo hijo no es "infinito", lo agregamos a la cola.
        if (f(child) < INT_MAX) {
          q.add(child);
        } 
      }
    }
  }
  // No hay solucion.
  // Imprimimos la cantidad de nodos expandidos por profundidad y la 
  // memoria virtual usada.
  print_time();
  cout << "Final memory: ";
  print_memory_used();
  print_visited();
  cout << "No hay solucion.\n";
  return NULL;
}



/*
  Implementacion de la busqueda en profundida de IDA*.

  INPUTS:
    unsigned bound            =>  Limite en el valor de un estado (costo + heuristica).
    unsigned g                =>  Costo del camino hasta el estado actual.
    unsigned d                =>  Profundidad.
    unsigned (*h) (state_t*)  =>  Heuristica.
    bool part_pruning         =>  Indica si se usara eliminacion parcial de duplicados.
  OUTPUT:
    pair<bool, int>  =>  El booleano indica si se encontro la solucion e int indica la cota
          obtenida en esta iteracion.
*/
pair<bool, unsigned> ida_search(
    unsigned bound, 
    unsigned g,
    unsigned d,
    unsigned (*h) (state_t*),
    bool part_pruning,
    int history
) {
  // Actualizamos el conteo de nodos.
  if (visited->size() <= d) visited->push_back(1);
  else (*visited)[d]++;

  unsigned h_value = h(state);
  unsigned f_value = g + h_value;
  if (f_value > bound) return {false, f_value};
  if (h_value == 0) return {true, f_value};

  unsigned t = UINT_MAX;

  // Para eliminacion parcial de duplicados.
  int child_hist;

  // Variables para iterar a traves de los sucesores de un determinado estado.
  ruleid_iterator_t iter;
  int ruleid;
  pair<bool, unsigned> p;

  init_fwd_iter(&iter, state);
  while((ruleid = next_ruleid(&iter)) >= 0) {
    // Eliminacion parcial de duplicados.
    if (part_pruning) {
      if (! fwd_rule_valid_for_history(history, ruleid)) continue;
      child_hist = next_fwd_history(history, ruleid);
    } else {
      child_hist = 0;
    }

    // Aplicamos la regla.
    apply_rule(ruleid, state);

    if (h(state) < UINT_MAX) {
      // Agregamos la regla al path.
      path.push_back(ruleid);
      p = ida_search(bound, g + get_fwd_rule_cost(ruleid), d+1, h, part_pruning, child_hist);
      if (p.first) return p;

      if (p.second < t) t = p.second;
      // Si no conseguimos una solucion, sacamos la regla del path.
      path.pop_back();
    }

    // Si no conseguimos una solucion, regresamos al estado anterior.
    revert_rule(ruleid, state);
  }
  return {false, t};
}

/* 
  Implementacion del algoritmo IDA*.

  INPUTS:
    state_t *s_init           =>  Estado inicial del problema.
    unsigned (*h) (state_t*)  =>  Heuristica.
    bool part_pruning         =>  Indica si se usara eliminacion parcial de duplicados.
  OUTPUT:
    vector<int> => Lista de acciones necesarias para alcanzar la meta. 
*/
vector<int> ida(
    state_t *s_init, 
    unsigned (*h) (state_t*),
    bool part_pruning
) {
  // Iniciamos el conteo del tiempo.
  start = clock();

  state = s_init;
  unsigned bound = h(state);
  pair<bool, unsigned> p;

  // Para eliminacion parcial de duplicados
  int hist = init_history;

  cout << "Thresholds: ";
  fflush(stdout);

  // Mientras nuestra cota no sea "infinita"/
  while (bound != UINT_MAX) {
    // Imprimimos la cota actual.
    cout << bound << " ";
    fflush(stdout);

    p = ida_search(bound, 0, 0, h, part_pruning, hist);

    if (p.first) {
      // Imprimimos la cantidad de nodos expandidos por profundidad y la 
      // memoria virtual usada.
      cout << "\n";
      print_time();
      cout << "Final memory: ";
      print_memory_used();
      print_visited();
      return path;
    }
    bound = p.second;
  }

  // Imprimimos la cantidad de nodos expandidos por profundidad y la 
  // memoria virtual usada.
  cout << "\n";
  print_time();
  cout << "Final memory: ";
  print_memory_used();
  print_visited();
  cout << "No hay solucion.\n";
  path.clear();
  return path;
}


/*
  Implementacion de la busqueda en profundida de IDA* con eliminacion de duplicados.

  INPUTS:
    Node *node                =>  Nodo desde donde se realizara la busqueda.
    set<uint64_t> *path       =>  Conjunto que contiene los estados desde la raiz hasta el 
        estado actual.
    int bound                 =>  Limite en el valor de un nodo (costo + heuristica).
    unsigned (*h) (state_t*)  =>  Heuristica.
  OUTPUT:
    pair<Node*, int>  =>  Contiene una referencia al nodo objetivo en caso de ser encontrado
        o el menor valor encontrado que supera el limite actual.
*/
pair<Node*, unsigned> ida_search_dup_pruning(
    Node *node, 
    set<uint64_t> *path, 
    unsigned bound, 
    unsigned (*h) (state_t*)
) {
  // Actualizamos el conteo de nodos.
  if (visited->size() <= node->d) visited->push_back(1);
  else (*visited)[node->d]++;

  unsigned f = node->g + h(node->state);
  if (f > bound) return {NULL, f};
  if (is_goal(node->state)) return {node, f};
  pair<Node*, unsigned> t;

  // Variables para iterar a traves de los sucesores de un determinado estado.
  state_t *succ;
  ruleid_iterator_t iter;
  Node* child;
  int ruleid;
  unsigned min = UINT_MAX;

  // Inicializamos el iterador de sucesores.
  init_fwd_iter(&iter, node->state);
  while((ruleid = next_ruleid(&iter)) >= 0) {

    // Obtenemos el siguiente sucesor y creamos al nodo hijo.
    succ = new state_t;
    apply_fwd_rule(ruleid, node->state, succ);
    child = node->make_node(succ, ruleid);

    // Si el estado no se encuentra en el camino actual.
    if (path->count(hash_state(child->state)) == 0) {
      // Lo agregamos y llamamos recursivamente a la funcion.
      path->insert(hash_state(child->state));
      t = ida_search_dup_pruning(child, path, bound, h);

      // Si encontramos una solucion la retornamos, si no sacamos al estado del
      // camino y probamos con otro sucesor.
      if (t.first != NULL) return t;
      if (t.second < min) min = t.second;
      path->erase(hash_state(child->state));

    }

    // Liberamos la memoria del nodo hijo
    delete child;
  }
  return {NULL, min};
}

/*
  Implementacion del algoritmo IDA* con eliminacion de duplicados.

  INPUTS:
    state_t *s_init           =>  Estado inicial del problema.
    unsigned (*h) (state_t*)  =>  Heuristica.
  OUTPUT:
    Node* => Direccion del nodo solucion. NULL si no se consiguio ninguna.
*/
Node *ida_dup_pruning(
    state_t *s_init, 
    unsigned (*h) (state_t*)
) {
  // Iniciamos el conteo del tiempo.
  start = clock();

  Node *root = new Node(s_init);
  unsigned bound = h(root->state);

  // path es un conjunto que almacena los nodos del camino desde la raiz
  // hasta el nodo actual. Usamos un conjunto para discriminar facilmente 
  // si un nodo se encuentra en el camino actual.
  set<uint64_t> *path = new set<uint64_t>;
  path->insert(hash_state(root->state));

  pair<Node*, unsigned> t;

  cout << "Thresholds: ";
  fflush(stdout);

  while (bound != UINT_MAX) {
    // Imprimimos la cota actual.
    cout << bound << " ";
    fflush(stdout);

    t = ida_search_dup_pruning(root, path, bound, h);

    if (t.first != NULL) {
      // Imprimimos la cantidad de nodos expandidos por profundidad y la 
      // memoria virtual usada.
      cout << "\n";
      print_time();
      cout << "Final memory: ";
      print_memory_used();
      print_visited();
      return t.first;
    }
    
    bound = t.second;
  }

  // Imprimimos la cantidad de nodos expandidos por profundidad y la 
  // memoria virtual usada.
  cout << "\n";
  print_time();
  cout << "Final memory: ";
  print_memory_used();
  print_visited();
  cout << "No hay solucion.\n";
  return NULL;
}



