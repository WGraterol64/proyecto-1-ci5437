/*
  Implementacion de las heuristicas usadas en el proyecto.
*/
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <dirent.h>

#define MAX_PDB_NAME 257
#define MAX_STATE_LEN 513
#define DIM 4

using namespace std;

// Distancia manhattan.
unsigned manhattan(state_t *state);
// Inicializa los pdbs que se usaran en la heuristica.
void init_pdbs(char *dir_name);
// Obtenemos la suma de las heuristicas pdb para 15puzzle.
unsigned additive_pdb_15puzzle(state_t *state);
// Obtenemos la suma de las heuristicas pdb para 24puzzle.
unsigned additive_pdb_24puzzle(state_t *state);
// Obtenemos la suma de las heuristicas pdb para las torres de hanoi 4P 12D.
unsigned additive_pdb_hanoi12D(state_t *state);