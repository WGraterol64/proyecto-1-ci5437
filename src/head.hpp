#include <iostream>
#include "Node.hpp"
#include "InformedSearchs.hpp"

using namespace std;

#define MAX_LINE_LENGTH 255

/* Valor absoluto. */
int abs(int x);

/* Heuristica. */
int h(state_t *s);

// Funcion de evaluacion.
int f(Node *node);