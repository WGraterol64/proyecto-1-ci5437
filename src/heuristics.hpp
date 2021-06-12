/*
  Implementacion de las heuristicas usadas en el proyecto.
*/
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <dirent.h>
#include <algorithm>

#define MAX_PDB_NAME 257
#define MAX_STATE_LEN 513
#define DIM 4

using namespace std;

// Distancia manhattan.
unsigned manhattan(state_t *state);

// Para establecer si las pdbs se sumaran o se tomara el maximo.
void set_max(void);
void set_sum(void);
// Inicializa los pdbs que se usaran en la heuristica.
void init_pdbs(char *dir_name);

// Para establecer cual NPuzzle se resolvera.
void set_15puzzle(void);
void set_24puzzle(void);
// Heuristica con pdbs para el NPuzzle.
unsigned pdb_Npuzzle(state_t *state);

// Para establecer cuantos discos se usaran en la torre de hanoi.
void set_hanois12D(void);
void set_hanois14D(void);
void set_hanois18D(void);
// Heuristica con pdbs para las torres de hanoi.
unsigned pdb_hanois(state_t *state);

// Para establecer cuantos discos se usaran en la torre de hanoi.
void set_topspin12(void);
void set_topspin14(void);
void set_topspin17(void);
// Heuristica con pdbs para las torres de hanoi.
unsigned pdb_topspin(state_t *state);

// Heuristica con pdbs para el cubo de rubik.
unsigned pdb_rubik(state_t *state);