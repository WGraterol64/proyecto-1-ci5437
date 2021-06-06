#include "InformedSearchs.hpp"

/*
  Busqueda de mejor camino.

  INPUTS:
    state_t *s_init             =>  Estado inicial de la busqueda.
    unsigned (*f) (Node*)       =>  Funcion de evaluacion de un nodo. Si f es el costo del 
        camino parcial, entonces el algoritmo es UCS; si f es una heuristica, entonces 
        el algoritmo es de busqueda voraz; y si es el costo + heuristica, entonces es A*
    vector<unsigned> *visited   =>  Lista que cuenta el numero de estados visitados 
        por nivel de profundidad.
  OUTPUT:
    Node* =>  Nodo que contiene la solucion. NULL si no hay solucion.
*/
Node *best_first_search(
    state_t *s_init, 
    unsigned (*f) (Node*),
    vector<unsigned> *visited
) {
  // En q almacenaremos los nodos que representan los estados a explorar
  // ordenados segun f. Primero agregamos el estado inicial.
  PriorityQueue<unsigned, Node*> q = PriorityQueue<unsigned, Node*>(f);
  Node* node = new Node(s_init);
  q.add(node);

  // Variables para iterar a traves de los sucesores de un determinado estado.
  state_t *succ;
  ruleid_iterator_t iter;
  int ruleid;

  while (1) {
    // Si q se queda vacio, no hay solucion.
    if (q.empty()) return NULL;
    // Obtenemos el siguiente nodo y verificamos si es solucion.
    node = q.pop();
    if (visited->size() <= node->d) visited->push_back(1);
    else (*visited)[node->d]++;
    if (is_goal(node->state)) return node;

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
    vector<unsigned> *visited   =>  Lista que cuenta el numero de estados visitados 
        por nivel de profundidad.
  OUTPUT:
    Node* =>  Nodo que contiene la solucion. NULL si no hay solucion.
*/
Node *best_first_search_dup_pruning(
    state_t *s_init, 
    unsigned (*f) (Node*),
    vector<unsigned> *visited
) {
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

  while (1) {
    // Si frontier se queda vacio, no hay solucion.
    if (frontier.empty()) return NULL;
    // Obtenemos el siguiente nodo y verificamos si es solucion.
    node = frontier.pop();
    if (visited->size() <= node->d) visited->push_back(1);
    else (*visited)[node->d]++;
    if (is_goal(node->state)) return node;
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
    vector<unsigned> *visited   =>  Lista que cuenta el numero de estados visitados 
        por nivel de profundidad.
  OUTPUT:
    Node* =>  Nodo que contiene la solucion. NULL si no hay solucion.
*/
Node *best_first_search_late_dup_pruning(
    state_t *s_init, 
    unsigned (*f) (Node*), 
    vector<unsigned> *visited
) {
  // En q almacenaremos los nodos que representan los estados a explorar
  // ordenados segun f. Primero agregamos el estado inicial.
  PriorityQueue<unsigned, Node*> q = PriorityQueue<unsigned, Node*>(f);
  Node* node = new Node(s_init);
  q.add(node);

  // En explored almacenaremos los nodos visitados, su color y su costo parcial.
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
    if (visited->size() <= node->d) visited->push_back(1);
    else (*visited)[node->d]++;

    // Si el estado actual no ha sido visitado o su costo parcial es menor que el 
    // almacenado en explored.
    if (explored.count(hash_value) == 0 || node->g < explored[hash_value]) {
      if (is_goal(node->state)) return node;
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
  return NULL;
}

/*
  Implementacion de la busqueda en profundida de IDA* con eliminacion de duplicados.

  INPUTS:
    Node *node                =>  Nodo desde donde se realizara la busqueda.
    set<state_t*> *path       =>  Conjunto que contiene los estados desde la raiz hasta el 
        estado actual.
    int bound                 =>  Limite en el valor de un nodo (costo + heuristica).
    unsigned (*h) (state_t*)  =>  Heuristica.
    vector<unsigned> *visited =>  Lista que cuenta el numero de estados visitados 
        por nivel de profundidad.
  OUTPUT:
    pair<Node*, int>  =>  Contiene una referencia al nodo objetivo en caso de ser encontrado
        o el menor valor encontrado que supera el limite actual.
*/
pair<Node*, unsigned> ida_search_dup_pruning(
    Node *node, 
    set<state_t*> *path, 
    unsigned bound, 
    unsigned (*h) (state_t*),
    vector<unsigned> *visited
) {
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
    if (path->count(child->state) == 0) {
      // Lo agregamos y llamamos recursivamente a la funcion.
      path->insert(child->state);
      t = ida_search_dup_pruning(child, path, bound, h, visited);

      // Si encontramos una solucion la retornamos, si no sacamos al estado del
      // camino y probamos con otro sucesor.
      if (t.first != NULL) return t;
      if (t.second < min) min = t.second;
      path->erase(child->state);
      // Liberamos la memoria del nodo hijo
      delete child;
    }
  }
  return {NULL, min};
}

/*
  Implementacion del algoritmo IDA* con eliminacion de duplicados.

  INPUTS:
    state_t *s_init           =>  Estado inicial del problema.
    unsigned (*h) (state_t*)  =>  Heuristica.
    vector<unsigned> *visited =>  Lista que cuenta el numero de estados visitados 
        por nivel de profundidad.
  OUTPUT:
    Node* => Direccion del nodo solucion. NULL si no se consiguio ninguna.
*/
Node *ida_dup_pruning(
    state_t *s_init, 
    unsigned (*h) (state_t*),
    vector<unsigned> *visited
) {
  Node *root = new Node(s_init);
  unsigned bound = h(root->state);
  set<state_t*> *path = new set<state_t*>;
  path->insert(root->state);
  pair<Node*, unsigned> t;

  while (1) {
    t = ida_search_dup_pruning(root, path, bound, h, visited);
    if (t.first != NULL) return t.first;
    if (t.second == INT_MAX) return NULL;
    bound = t.second;
    cout << "Bound: " << bound << "\n";
  }
}

state_t *state;
vector<int> path;
/*
  Implementacion de la busqueda en profundida de IDA* con eliminacion parcial de duplicados.

  INPUTS:
    unsigned bound            =>  Limite en el valor de un estado (costo + heuristica).
    unsigned g                =>  Costo del camino hasta el estado actual.
    unsigned d                =>  Profundidad.
    unsigned (*h) (state_t*)  =>  Heuristica.
    vector<unsigned> *visited =>  Lista que cuenta el numero de estados visitados 
        por nivel de profundidad.
  OUTPUT:
    pair<bool, int>  =>  El booleano indica si se encontro la solucion e int indica la cota
          obtenida en esta iteracion.
*/
pair<bool, unsigned> ida_search_part_dup_pruning(
    unsigned bound, 
    unsigned g,
    unsigned d,
    unsigned (*h) (state_t*),
    vector<unsigned> *visited
) {
  if (visited->size() <= d) visited->push_back(1);
  else (*visited)[d]++;
  unsigned h_value = h(state);
  unsigned f_value = g + h_value;
  if (f_value > bound) return {false, f_value};
  if (h_value == 0) return {true, f_value};

  unsigned t = UINT_MAX;

  // Variables para iterar a traves de los sucesores de un determinado estado.
  ruleid_iterator_t iter;
  state_t *state_aux;
  int ruleid;
  pair<bool, unsigned> p;
  unsigned cost;

  init_fwd_iter(&iter, state);
  while((ruleid = next_ruleid(&iter)) >= 0) {
    // Eliminacion parcial de duplicados.
    //if( fwd_rule_valid_for_history(history, ruleid) == 0 ) continue;
    cost = g + get_fwd_rule_cost(ruleid);

    // Aplicamos la regla.
    // Resulta que no podemos ejecutar:   apply_fwd_rule(ruleid, state, state)
    // porque state se modifica mientras se lee (que implementacion tan chimba)
    // por lo tanto, debemos usar una copia auxiliar para que no hayan errores.
    // Este peo me amargo la vida como por 2 horas.
    state_aux = new state_t;
    copy_state(state_aux, state);
    apply_fwd_rule(ruleid, state_aux, state);
    delete state_aux;

    if (h(state) < UINT_MAX) {
      // Agregamos la regla al path.
      path.push_back(ruleid);
      p = ida_search_part_dup_pruning(bound, cost, d+1, h, visited);
      if (p.first) return p;

      if (p.second < t) t = p.second;
      // Si no conseguimos una solucion, sacamos la regla del path.
      path.pop_back();
    }

    // Si no conseguimos una solucion, regresamos al estado anterior.
    state_aux = new state_t;
    copy_state(state_aux, state);
    apply_bwd_rule(ruleid, state_aux, state);
    delete state_aux;
  }
  return {false, t};
}


/* 
  Implementacion del algoritmo IDA* con eliminacion parcial de duplicados.

  INPUTS:
    state_t *s_init           =>  Estado inicial del problema.
    unsigned (*h) (state_t*)  =>  Heuristica.
    vector<unsigned> *visited =>  Lista que cuenta el numero de estados visitados 
        por nivel de profundidad.
  OUTPUT:
    vector<int> => Lista de acciones necesarias para alcanzar la meta. 
*/
vector<int> ida_part_dup_pruning(
    state_t *s_init, 
    unsigned (*h) (state_t*),
    vector<unsigned> *visited
) {
  state = s_init;
  unsigned bound = h(state);
  pair<bool, unsigned> p;

  // Mientras nuestra cota no sea "infinita"/
  while (true) {
    p = ida_search_part_dup_pruning(bound, 0, 0, h, visited);
    if (p.first) return path;
    bound = p.second;
  }
}