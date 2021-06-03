#pragma once 

#include <set>

using namespace std;

/* 
Implementacion de una estructura que almacena elementos ordenados segun una
funcion y permite las siguientes
operaciones:
  * Verificar si esta vacia en O(1).
  * Agregar un elemento en O(log n).
  * Obtener el elemento de menor valor en O(log n).
*/
template <typename C, typename E>
class PriorityQueue {
  private:
    // ordered_elems contiene los elementos ordenados segun su costo.
    set<pair<C, E>> ordered_elems;
    // Funcion de evaluacion de elementos.
    C (*f) (E);

  public:
    // Constructor.
    PriorityQueue(C (*f) (E));

    // Indica si la estructura esta vacia.
    bool empty(void);
    // Agrega un elemento a la estructura.
    void add(E elem);
    // Retorna el elemento de menor valor de la estructura.
    E pop(void);
};