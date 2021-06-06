/*
  Este archivo contiene las implementaciones de los algoritmos de busqueda informada.
*/
#pragma once 

#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <bits/stdc++.h>
#include "sys/types.h"
#include "sys/sysinfo.h"

#include "Node.hpp"
#include "PriorityQueue.hpp"
#include "NodesPriorityQueue.hpp"

using namespace std;

// Busqueda de mejor camino.
Node *best_first_search(
  state_t *s_init, 
  unsigned (*f) (Node*)
);

// Busqueda de mejor camino con eliminacion de duplicados.
Node *best_first_search_dup_pruning(
  state_t *s_init, 
  unsigned (*f) (Node*)
);

// Busqueda de mejor camino con eliminacion tardia de duplicados.
Node *best_first_search_late_dup_pruning(
  state_t *s_init, 
  unsigned (*f) (Node*)
);

// Implementacion del algoritmo IDA*.
vector<int> ida(
  state_t *s_init, 
  unsigned (*h) (state_t*),
  bool part_pruning
);

// Implementacion del algoritmo IDA* con eliminacion de duplicados.
Node *ida_dup_pruning(
  state_t *s_init, 
  unsigned (*h) (state_t*)
);