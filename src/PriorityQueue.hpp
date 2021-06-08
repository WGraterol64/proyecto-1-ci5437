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

/* 
  Agregamos las implementaciones en este archivo y no en un .cpp debido a los 
  problemas de compilacion usando templates.
*/

/* Constructor. */
template <typename C, typename E>
PriorityQueue<C, E>::PriorityQueue(C (*f) (E)) {
  this->f = f;
}

/* 
  Verifica si la estructura esta vacia.

  OUTPUT:
    bool  =>  True si la estructura esta vacia.
  O(1)
*/
template <typename C, typename E>
bool PriorityQueue<C, E>::empty(void) {
  return this->ordered_elems.size() == 0;
}

/*
  Agrega un elemento a la estructura.

  INPUT:
    E elem  =>  Elemento a agregar
  O(log n)
*/
template <typename C, typename E>
void PriorityQueue<C, E>::add(E elem) {
  // Agregamos el par (f(elem), elem) al conjunto ordenado.
  this->ordered_elems.insert(pair<int, E> (this->f(elem), elem));
}

/* 
  Retorna el elemento de menor valor de la estructura.

  OUTPUT:
    E =>  Elemento de menor valor de la estructura.
  O(log n)
*/
template <typename C, typename E>
E PriorityQueue<C, E>::pop(void) {
  // Obtenemos el menor elemento de la lista de elementos ordenados.
  typename set<pair<C, E>>::iterator it = this->ordered_elems.begin();
  // Eliminamos dicho elemento tanto de la lista ordenada.
  this->ordered_elems.erase(it);
  return (*it).second;
}