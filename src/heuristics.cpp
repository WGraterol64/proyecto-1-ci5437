#include "heuristics.hpp"

/* Valor absoluto. */
unsigned abs(unsigned x) { 
  if (x < 0) return -x;
  return x;
}

/* Distancia Manhattan. */
unsigned manhattan(state_t *state) { 
  // Obtenemos la representacion del estado en char*.
  char s[100];
  sprint_state(s, 100, state);
  char *token, *rest = s;
  int val, result = 0, index = 0;

  // Obtenemos cada token del estado.
  while ((token = strtok_r(rest, " ", &rest))) {
    // Ignoramos la casilla blanca.
    if (strcmp(token, "B") == 0)  {
      index++;
      continue;
    }

    val = atoi(token);
    // La posicion (x, y) del i-esimo token sera (i/N, i%N)
    result += abs(index/DIM - val/DIM) + abs(index%DIM - val%DIM);
    index++;
  }
  return (unsigned) result; 
}