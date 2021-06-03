#pragma once 

#include <vector>

using namespace std;

/* Implementacion de un nodo en un grafo implicito. */
class Node {
  public:
    // Constructor.
    Node(state_t *state, Node *parent=NULL, int rule_id=-1, int g=0);
    // Destructor.
    ~Node(void);

    // Estado que representa.
    state_t *state;
    // Padre del nodo actual 
    Node *parent;
    // Accion necesaria para pasar del nodo padre al nodo actual.
    int rule_id;
    // Costo del camino hasta el nodo actual. 
    int g;

    // Retorna la lista de acciones necesarias para pasar desde el nodo raiz
    // hasta el nodo actual, pero invertida
    vector<int> extract_path(void);
    // Crea un nodo a partir del nodo actual.
    Node *make_node(state_t *state, int rule_id);
};