#include "heuristics.hpp"

/* ===================== VARIABLES GLOBALES ===================== */
// Almacenara cada pdb del puzzle.
vector<state_map_t*> pdbs;
// Particion del 15puzzle
string partition[4][4] = {
  {"10", "11", "14", "15"},
  {"2", "3", "6", "7"}, 
  {"1", "4", "5", "-1"}, 
  {"8", "9", "12", "13"}
};

/* ===================== FUNCIONES AUXILIARES ===================== */
/* Valor absoluto. */
unsigned abs(unsigned x) { 
  if (x < 0) return -x;
  return x;
}

/* Inicializa los pdbs que se usaran en la heuristica. */
void init_pdbs(char *dir_name) {
  DIR *dir;
  struct dirent *pdb_file;
  char path[MAX_PDB_NAME];
  FILE *f;

  // Abrimos el directorio que contiene los pdbs.
  if ((dir = opendir(dir_name)) != NULL) {

    // Por cada archivo en el directorio.
    while ((pdb_file = readdir(dir)) != NULL) {

      // Verificamos que el archivo no sea . ni ..
      if (strcmp(pdb_file->d_name, ".") != 0 && strcmp(pdb_file->d_name,  "..") != 0) {
        // Almacenamos un nuevo pdb.
        sprintf(path, "%s/%s", dir_name, pdb_file->d_name);
        cout << "Opening " << path << "\n";
        f = fopen(path, "r");
        pdbs.push_back(read_state_map(f));
      }
    }
    closedir(dir);

  } else {
    /* could not open directory */
    cout << "No se pudo abrir el directorio '\e[1m" << dir_name << "\e[0m'.\n";
    exit(1);
  }
}

/*
  Crea un estado abstracto indicandole la particion de tokens que se mantendran
  y el token por defecto que sustituira al resto.

  INPUTS:
    char *state           =>  Estado original.
    string *block         =>  Lista de tokens que se mantendran igual.
    int block_len         =>  Longitud de la lista de bloques.
    string default_token  =>  Token por defecto que sustituira a los que no se 
        encuentran en la particion
  OUTPUT:
    char* =>  Estado abstraido.
*/
char *make_state_abs(char *state, string *block, int block_len, string default_token) {
  // Copiamos el estado.
  char *state_copy = (char*) calloc(MAX_STATE_LEN, sizeof(char));
  strcpy(state_copy, state);

  string state_abs = "";
  bool find;

  // Obtenemos el primer token.
  char *token = strtok(state_copy, " ");

  // Por cada token.
  while (token != NULL) {
    // Verificamos si se encuentra en el bloque.
    find = false;
    for (int i = 0; i < block_len; i++) {
      // Si es asi, lo agregamos al estado abstraido.
      if (strcmp(token, block[i].c_str()) == 0) {
        string str(token);
        state_abs += str;
        find = true;
        break;
      } 
    }
    // Si no, agregamos el token por defecto.
    if (! find) {
      state_abs += default_token;
    }
    // Agregamos un espacio para separar lo tokens.
    state_abs += " ";

    token = strtok(NULL, " ");
  }
  strcpy(state_copy, state_abs.c_str());
  return state_copy;
}



/* ===================== HEURISTICAS ===================== */
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


/* Additive pdbs. */
unsigned additive_pdb_15puzzle(state_t *state) {
  state_t *state_abs = new state_t;
  unsigned h_value = 0;
  int index = 0;
  char *state_str = (char*) calloc(MAX_STATE_LEN, sizeof(char)); 
  char *state_abs_str;

  // Guardamos el estado en un string.
  sprint_state(state_str, 100, state);

  // Sumamos las heuristicas.
  for (vector<state_map_t*>::iterator it = pdbs.begin(); it != pdbs.end(); it++) {
    // Guardamos el estado en un string.
    sprint_state(state_str, 100, state);

    // Creamos un estado a partir de la abstraccion.
    state_abs_str = make_state_abs(state_str, partition[index++], 4, "B");
    read_state(state_abs_str, state_abs);
    free(state_abs_str);

    // Agregamos el valor del estado abstraido.
    h_value += (unsigned) *state_map_get(*it, state_abs);
  }
  free(state_str);
  return h_value;
}

/* Non-additive pdbs. */
unsigned non_additive_pdb(state_t *state) {
  unsigned h_i, h = 0;
  // Obtenemos el maximo de las heuristicas.
  for (vector<state_map_t*>::iterator it = pdbs.begin(); it != pdbs.end(); it++) {
    h_i = (unsigned) *state_map_get(*it, state);
    if (h_i > h) h = h_i; 
  }
  return h;
}