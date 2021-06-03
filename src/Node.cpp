#include "Node.hpp"

/* Constructor. */
Node::Node(state_t *state, Node *parent, int rule_id, int g, long unsigned int d) {
  this->state = state;
  this->parent = parent;
  this->rule_id = rule_id;
  this->g = g;
  this->d = d;
}

/* Destructor. */
Node::~Node(void) {
  delete this->state;
}

/* 
Retorna la lista de acciones necesarias para pasar desde el nodo raiz hasta el 
nodo actual, pero invertida.
OUTPUT: 
  vector<int>  =>  Lista de acciones.
*/
vector<int> Node::extract_path(void) {
  vector<int> rules;
  Node *node = this;
  while (node->parent != NULL) {
    rules.push_back(node->rule_id);
    node = node->parent;
  }
  return rules;
}

/*
Crea un nodo a partir del nodo actual.

INPUTS:
  state_t *state  =>  Estado que representa el nuevo nodo.
  int rule_id     =>  Accion para pasar del nodo actual al nuevo nodo.
OUTPUT:
  Node* =>  Nuevo nodo.
*/
Node* Node::make_node(state_t *state, int rule_id) {
  return new Node(state, this, rule_id, this->g + get_fwd_rule_cost(rule_id), this->d+1);
}
