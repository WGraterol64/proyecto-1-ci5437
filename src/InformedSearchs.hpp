#pragma once 

#include <vector>
#include <set>
#include <map>
#include "Node.hpp"
#include "NodesPriorityQueue.hpp"

using namespace std;

/*
Busqueda de mejor camino.

INPUTS:
  state_t *s_init   =>  Estado inicial de la busqueda.
  int (*f) (Node*)  =>  Funcion de evaluacion de un nodo. Si f es el costo del camino
      parcial, entonces el algoritmo es UCS; si f es una heuristica, entonces el 
      algoritmo es de busqueda voraz; y si es el costo + heuristica, entonces es A*
OUTPUT:
  Node* =>  Nodo que contiene la solucion. NULL si no hay solucion.
*/
Node *best_first_search(state_t *s_init, int (*f) (Node*)) {
  // En frontier almacenaremos los nodos que representan los estados a explorar
  // ordenados segun f. Primero agregamos el estado inicial.
  NodesPriorityQueue frontier = NodesPriorityQueue(f);
  Node* node = new Node(s_init);
  frontier.add(node);
  // En explored almacenaremos los nodos visitados
  set<uint64_t> explored;
  vector<pair<state_t*, int>> *successors;
  Node* child;
  // Variables para iterar a traves de los sucesores de un determinado estado.
  state_t *succ;
  ruleid_iterator_t iter;
  int ruleid;

  while (1) {
    // Si frontier se queda vacio, no hay solucion.
    if (frontier.empty()) return NULL;
    // Obtenemos el siguiente nodo y verificamos si es solucion.
    node = frontier.pop();
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

    // Obtenemos el siguiente sucesor.
    succ = new state_t;
    apply_fwd_rule(ruleid, node->state, succ);
    // Creamos el nodo hijo.
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