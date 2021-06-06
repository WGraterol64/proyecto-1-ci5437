/*
  Este archivo contiene las implementaciones de los algoritmos de busqueda informada.
*/
#pragma once 

#include <vector>
#include <set>
#include <map>
#include <iostream>
#include "Node.hpp"
#include "PriorityQueue.hpp"
#include "NodesPriorityQueue.hpp"

using namespace std;

// Busqueda de mejor camino.
Node *best_first_search(
  state_t *s_init, 
  unsigned (*f) (Node*),
  vector<unsigned> *visited
);

// Busqueda de mejor camino con eliminacion tardia de duplicados.
Node *best_first_search_dup_pruning(
  state_t *s_init, 
  unsigned (*f) (Node*),
  vector<unsigned> *visited
);

// Busqueda de mejor camino con eliminacion tardia de duplicados.
Node *best_first_search_late_dup_pruning(
  state_t *s_init, 
  unsigned (*f) (Node*), 
  vector<unsigned> *visited
);

// Implementacion del algoritmo IDA* con eliminacion de duplicados.
Node *ida_dup_pruning(
  state_t *s_init, 
  unsigned (*h) (state_t*),
  vector<unsigned> *visited
);

// Implementacion del algoritmo IDA* con eliminacion parcial de duplicados.
vector<int> ida_part_dup_pruning(
  state_t *s_init, 
  unsigned (*h) (state_t*),
  vector<unsigned> *visited
);