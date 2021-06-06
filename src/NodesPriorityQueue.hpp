#pragma once 

#include <vector>
#include <set>
#include <map>
#include <cstdint>
#include "Node.hpp"

using namespace std;

/* 
Implementacion de una estructura que almacena nodos y permite las siguientes
operaciones:
  * Verificar si esta vacia en O(1).
  * Verificar si un nodo esta en la estructura en O(1).
  * Agregar un nodo en O(log n).
  * Obtener el nodo de menor valor en O(log n).
  * Intercambiar un nodo por otro que representa el mismo estado pero tiene 
    mayor costo en O(log n).
*/
class NodesPriorityQueue {
  private:
    // hash mapea los id de los nodos a pares valor, nodo.
    map<uint64_t, pair<int, Node*>> hash;
    // ordered_nodes contiene los nodos ordenados segun algun valor.
    set<pair<int, Node*>> ordered_nodes;
    // Funcion de evaluacion de nodos.
    int (*f) (Node*);

  public:
    // Constructor.
    NodesPriorityQueue(int (*f) (Node*));

    // Indica si la estructura esta vacia.
    bool empty(void);
    // Verifica si un nodo se encuentra en la estructura.
    bool find(Node *node);
    // Agrega un nodo a la estructura.
    void add(Node *node);
    // Retorna el nodo de menor valor de la estructura.
    Node* pop(void);
    // Reemplaza un nodo si ya se encuentra en la estructura pero con menor valor.
    void replace_if_less(Node *node);
}; 