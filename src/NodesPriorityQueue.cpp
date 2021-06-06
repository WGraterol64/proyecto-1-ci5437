#include "NodesPriorityQueue.hpp"

/* Constructor. */
NodesPriorityQueue::NodesPriorityQueue(int (*f) (Node*)) {
  this->f = f;
}

/* 
  Verifica si la estructura esta vacia.
  OUTPUT:
  bool  =>  True si la estructura esta vacia.
  O(1)
*/
bool NodesPriorityQueue::empty(void) {
  return this->ordered_nodes.size() == 0;
}

/*
  Verifica si un nodo se encuentra en la estructura.
  OUTPUT:
    bool  =>  True si el nodo esta en la estructura.
  O(1)
*/
bool NodesPriorityQueue::find(Node *node) {
  return this->hash.count(hash_state(node->state)) != 0;
}

/*
  Agrega un nodo a la estructura.
  INPUT:
    Node* node  =>  Nodo a agregar
  O(log n)
*/
void NodesPriorityQueue::add(Node *node) {
  // value_node contiene el par (f(node), node)
  pair<int, Node*> value_node = pair<int, Node*>(this->f(node), node);
  // Agregamos el par (id : (f(node), node)) a la tabla de hash.
  this->hash.insert(pair<uint64_t, pair<int, Node*>> (hash_state(node->state), value_node));
  // Agregamos el par (f(node), node) al conjunto ordenado.
  this->ordered_nodes.insert(pair<int, Node*> (this->f(node), node));
}

/* 
  Retorna el nodo de menor valor de la estructura.
  OUTPUT:
    Node* =>  Nodo de menor valor de la estructura.
  O(log n)
*/
Node* NodesPriorityQueue::pop(void) {
  // Obtenemos el menor nodo de la lista de nodos ordenados.
  set<pair<int, Node*>>::iterator it = this->ordered_nodes.begin();
  // Eliminamos dicho nodo tanto de la lista ordenada como del hash.
  this->ordered_nodes.erase(it);
  this->hash.erase(hash_state((*it).second->state));
  // Retornamos el nodo.
  return (*it).second;
}

/*
  Reemplaza un nodo si ya se encuentra en la estructura pero con menor valor.
  INPUT:
    Node *node  =>  Node que posiblemente reemplazara a su equivalente.
  O(log n)
*/
void NodesPriorityQueue::replace_if_less(Node *node) {
  // Buscamos el nodo en el hash.
  map<uint64_t, pair<int, Node*>>::iterator it = this->hash.find(hash_state(node->state));
  // Si el nodo almacenado tiene un costo menor o igual al del nuevo nodo, no
  // se realiza el reemplazo
  if ((*it).second.second->g <= node->g) return;
  // Si no, realizamos el reemplazo.
  // value_node contiene el par (f(node), node)
  pair<int, Node*> value_node = pair<int, Node*>(this->f(node), node);
  this->hash[hash_state(node->state)] = value_node; 
  this->ordered_nodes.erase((*it).second);
  this->ordered_nodes.insert(value_node);
} 