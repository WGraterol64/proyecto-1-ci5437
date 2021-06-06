#pragma once 

#include <vector>
#include <set>
#include <map>
#include "Node.hpp"
#include "PriorityQueue.hpp"

using namespace std;

/*
Busqueda de mejor camino con eliminacion tardia de duplicados.

INPUTS:
  state_t *s_init       =>  Estado inicial de la busqueda.
  int (*f) (Node*)      =>  Funcion de evaluacion de un nodo. Si f es el costo del camino
      parcial, entonces el algoritmo es UCS; si f es una heuristica, entonces el 
      algoritmo es de busqueda voraz; y si es el costo + heuristica, entonces es A*
  vector<int> *visited  =>  Lista que cuenta el numero de estados visitados por nivel de 
      profundidad.
OUTPUT:
  Node* =>  Nodo que contiene la solucion. NULL si no hay solucion.
*/
Node *best_first_search(state_t *s_init, int (*f) (Node*), vector<int> *visited) {
  // En q almacenaremos los nodos que representan los estados a explorar
  // ordenados segun f. Primero agregamos el estado inicial.
  PriorityQueue<int, Node*> q = PriorityQueue<int, Node*>(f);
  Node* node = new Node(s_init);
  q.add(node);

  // En explored almacenaremos los nodos visitados, su color y su costo parcial.
  map<uint64_t, int> explored;
  
  // Variables para iterar a traves de los sucesores de un determinado estado.
  vector<pair<state_t*, int>> *successors;
  state_t *succ;
  Node *child;
  ruleid_iterator_t iter;
  uint64_t hash_value;
  int ruleid;

  while (! q.empty()) {
    node = q.pop();
    hash_value = hash_state(node->state);
    if (visited->size() <= node->d) visited->push_back(node->d);
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
Implementacion de la busqueda en profundida de IDA* dado un limite de coste.

INPUTS:
  Node *node          =>  Nodo desde donde se realizara la busqueda.
  set<state_t*> *path =>  Conjunto que contiene los estados desde la raiz hasta el 
      estado actual.
  int bound           =>  Limite en el valor de un nodo (costo + heuristica).
  int (*h) (state_t*) =>  Heuristica.
OUTPUT:
  pair<Node*, int>  =>  Contiene una referencia al nodo objetivo en caso de ser encontrado
      o el menor valor encontrado que supera el limite actual.
*/
pair<Node*, int> ida_star_search(
    Node *node, 
    set<state_t*> *path, 
    int bound, 
    int (*h) (state_t*)
  ) {
  int f = node->g + h(node->state);
  if (f > bound) return {NULL, f};
  if (is_goal(node->state)) return {node, f};
  pair<Node*, int> t;

  // Variables para iterar a traves de los sucesores de un determinado estado.
  state_t *succ;
  ruleid_iterator_t iter;
  Node* child;
  int ruleid;
  int min = INT_MAX;

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
      t = ida_star_search(child, path, bound, h);

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
Implementacion del algoritmo IDA*

INPUTS:
  state_t *s_init     =>  Estado inicial del problema.
  int (*h) (state_t*) =>  Heuristica.
OUTPUT:
  Node* => Direccion del nodo solucion. NULL si no se consiguio ninguna.
*/
Node *ida_star(state_t *s_init, int (*h) (state_t*)) {
  Node *root = new Node(s_init);
  int bound = h(root->state);
  set<state_t*> *path = new set<state_t*>;
  path->insert(root->state);
  pair<Node*, int> t;

  while (1) {
    t = ida_star_search(root, path, bound, h);
    if (t.first != NULL) return t.first;
    if (t.second == INT_MAX) return NULL;
    bound = t.second;
    cout << "Bound: " << bound << "\n";
  }
}