#include "heuristics.hpp"

/* ===================== VARIABLES GLOBALES ===================== */
// Almacenara cada pdb del puzzle.
vector<state_map_t*> pdbs;
// f indicara si las pdbs se sumaran o se tomara el maximo.
unsigned (*f) (unsigned, unsigned);

// Almacenara la particion que se usara para NPuzzle.
string **partition_Npuzzle;
int dim;
// Particion del 15puzzle
string partition_15puzzle[4][4] = {
  {"10", "11", "14", "15"},
  {"2", "3", "6", "7"}, 
  {"1", "4", "5", "-1"}, 
  {"8", "9", "12", "13"}
};
// Particion del 24puzzle
string partition_24puzzle[5][5] = {
  {"1", "2", "5", "6", "7"},
  {"3", "4", "8", "9", "14"}, 
  {"10", "11", "12", "15", "16"}, 
  {"13", "18", "19", "23", "24"},
  {"17", "20", "21", "22", "-1"}
};

// Almacenara la particion que se usara para las torres de hanoi.
pair<int,int> *partition_hanois;
pair<int,int> partition_hanois12D[2] = {
  {24, 47},
  {0, 23}
};
pair<int,int> partition_hanois14D[2] = {
  {28, 55},
  {0, 27}
};
pair<int,int> partition_hanois18D[3] = {
  {48, 71},
  {24, 47},
  {0, 23}
};

// Almacenara la particion que se usara para top spin.
pair<int,int> *partition_topspin;
pair<int,int> partition_topspin12[2] = {
  {7, 12},
  {1, 6}
};
pair<int,int> partition_topspin14[2] = {
  {1, 7},
  {8, 14}
};
pair<int,int> partition_topspin17[3] = {
  {1, 6},
  {7, 12},
  {13, 17}
};

// Almacena la particion del cubo de rubik.
pair<bool, vector<string>> partition_rubik[6] = {
  {true, {"R", "G"}},
  {true, {"B", "Y"}},
  {true, {"O", "W"}},
  {false,  {"R", "G"}},
  {false,  {"B", "Y"}},
  {false,  {"O", "W"}}
};

/* ===================== FUNCIONES AUXILIARES ===================== */
unsigned abs(unsigned x) { return (x < 0)? -x : x; }

unsigned max_h(unsigned h, unsigned h_i) { return (h > h_i) ? h : h_i; }

unsigned sum_h(unsigned h, unsigned h_i) { return h + h_i; }

void set_max(void) { f = max_h; }

void set_sum(void) { f = sum_h; }

void add_state(state_map_t *state_map, char *entry) {
  // Obtenemos la representacion del estado en char*.
  char s[MAX_STATE_LEN];
  char *value, *state_str;
  state_t *state = new state_t;

  strcpy(s, entry);

  // Obtenemos el valor y estado.
  value = strtok_r(s, " ", &state_str);
  read_state(state_str, state);
  
  // Agregamos el estado al map.
  state_map_add(state_map, state, atoi(value));
  delete state;
}

state_map_t* make_state_map(FILE *pdb_file) {
  state_map_t *state_map = new_state_map();

  char *line = NULL;
  size_t len = 0;

  // Leemos linea por linea y cada entrada la agregamos al map.
  while (getline(&line, &len, pdb_file) != -1) {
    add_state(state_map, line);
  }
  fclose(pdb_file);

  return state_map;
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
        pdbs.push_back(make_state_map(f));
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
  Crea un estado abstracto para NPuzzle indicandole la particion de tokens que 
  se mantendran y el token por defecto que sustituira al resto.

  INPUTS:
    char *state           =>  Estado original.
    string *block         =>  Lista de tokens que se mantendran igual.
    int block_len         =>  Longitud de la lista de bloques.
    string default_token  =>  Token por defecto que sustituira a los que no se 
        encuentran en la particion
  OUTPUT:
    char* =>  Estado abstraido.
*/
char *make_state_abs_NPuzzle(char *state, string *block, int block_len, string default_token) {
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

void set_15puzzle(void) { partition_Npuzzle = (string**) partition_15puzzle; dim = 4; }

void set_24puzzle(void) { partition_Npuzzle = (string**) partition_24puzzle; dim = 5; }


/*
  Crea un estado abstracto para las torres de hanoi indicandole el bloque de tokens que 
  se mantendran.

  INPUTS:
    char *state           =>  Estado original.
    pair<int, int> block  =>  Bloque que se mantendra.
  OUTPUT:
    char* =>  Estado abstraido.
*/
char *make_state_abs_hanois(char *state, pair<int, int> block) {
  // Copiamos el estado.
  char *state_copy = (char*) calloc(MAX_STATE_LEN, sizeof(char));
  strcpy(state_copy, state);
  int index = 0;
  string state_abs = "";

  // Obtenemos el primer token.
  char *token = strtok(state_copy, " ");

  // Por cada token.
  while (token != NULL) {
    // Si es esta dentro del bloque, lo agregamos al estado abstraido.
    if (block.first <= index && index <= block.second) {
      string str(token);
      state_abs += str;
    } else {
      state_abs += "0";
    }
    // Agregamos un espacio para separar lo tokens.
    state_abs += " ";

    token = strtok(NULL, " ");
    index++;
  }
  strcpy(state_copy, state_abs.c_str());
  return state_copy;
}

void set_hanois12D(void) { partition_hanois = partition_hanois12D; }

void set_hanois14D(void) { partition_hanois = partition_hanois14D; }

void set_hanois18D(void) { partition_hanois = partition_hanois18D; }


/*
  Crea un estado abstracto para top spin indicandole el bloque de tokens que 
  se mantendran.

  INPUTS:
    char *state           =>  Estado original.
    pair<int, int> block  =>  Bloque que se mantendra.
  OUTPUT:
    char* =>  Estado abstraido.
*/
char *make_state_abs_topspin(char *state, pair<int, int> block) {
  // Copiamos el estado.
  char *state_copy = (char*) calloc(MAX_STATE_LEN, sizeof(char));
  strcpy(state_copy, state);
  string state_abs = "";
  int token_val;

  // Obtenemos el primer token.
  char *token = strtok(state_copy, " ");

  // Por cada token.
  while (token != NULL) {
    token_val = stoi(token);
    // Si es esta dentro del bloque, lo agregamos al estado abstraido.
    if (block.first <= token_val && token_val <= block.second) {
      string str(token);
      state_abs += str;
    } else {
      state_abs += "0";
    }
    // Agregamos un espacio para separar lo tokens.
    state_abs += " ";

    token = strtok(NULL, " ");
  }
  strcpy(state_copy, state_abs.c_str());
  return state_copy;
}

void set_topspin12(void) { partition_topspin = partition_topspin12; }

void set_topspin14(void) { partition_topspin = partition_topspin14; }

void set_topspin17(void) { partition_topspin = partition_topspin17; }


/*
  Crea un estado abstracto para el cubo de rubik indicandole si es o no
  una esquina y cuales colores estan permitidos.

  INPUTS:
    char *state                       =>  Estado original.
    pair<bool, vector<string>> block  =>  Par que indica si es ono una esquina
        y los colores permitidos.
  OUTPUT:
    char* =>  Estado abstraido.
*/
char *make_state_abs_rubik(char *state, pair<bool, vector<string>> block) {
  // Copiamos el estado.
  char *state_copy = (char*) calloc(MAX_STATE_LEN, sizeof(char));
  strcpy(state_copy, state);
  string state_abs = "";
  int index = 1;
  bool find;

  // Obtenemos el primer token.
  char *token = strtok(state_copy, " ");

  // Por cada token.
  while (token != NULL) {
    // Si es esta dentro del bloque, lo agregamos al estado abstraido.
    // Para ello verificamos si coincide con ser o no una esquina y si
    // es de uno de los colores del bloque.
    find = false;
    if (block.first == ((index++) % 2 == 0)) {
      string str(token);
      for (string color : block.second) {
        if (str == color) {
         state_abs += str;
         find = true;
         break;
        }
      }
      if (! find) {
        state_abs += "0";
      }
    } else {
      state_abs += "0";
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
  char s[MAX_STATE_LEN];
  sprint_state(s, MAX_STATE_LEN, state);
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


/* Heuristic with pdbs 15puzzle. */
unsigned pdb_Npuzzle(state_t *state) {
  state_t *state_abs = new state_t;
  unsigned h_value = 0;
  int index = 0;
  char *state_str = (char*) calloc(MAX_STATE_LEN, sizeof(char)); 
  char *state_abs_str;

  // Sumamos las heuristicas.
  for (vector<state_map_t*>::iterator it = pdbs.begin(); it != pdbs.end(); it++) {
    // Guardamos el estado en un string.
    sprint_state(state_str, MAX_STATE_LEN, state);

    // Creamos un estado a partir de la abstraccion.
    state_abs_str = make_state_abs_NPuzzle(state_str, partition_Npuzzle[index++], dim, "B");
    read_state(state_abs_str, state_abs);
    free(state_abs_str);

    // Agregamos el valor del estado abstraido.
    h_value = f(h_value, (unsigned) *state_map_get(*it, state_abs));
  }
  free(state_str);
  return h_value;
}


/* Heuristic with pdbs towers of hanoi 4P 12D. */
unsigned pdb_hanois(state_t *state) {
  state_t *state_abs = new state_t;
  unsigned h_value = 0;
  int index = 0;
  char *state_str = (char*) calloc(MAX_STATE_LEN, sizeof(char)); 
  char *state_abs_str;

  // Sumamos las heuristicas.
  for (vector<state_map_t*>::iterator it = pdbs.begin(); it != pdbs.end(); it++) {
    // Guardamos el estado en un string.
    sprint_state(state_str, MAX_STATE_LEN, state);

    // Creamos un estado a partir de la abstraccion.
    state_abs_str = make_state_abs_hanois(state_str, partition_hanois[index++]);
    read_state(state_abs_str, state_abs);
    free(state_abs_str);

    // Agregamos el valor del estado abstraido.
    h_value = f(h_value, (unsigned) *state_map_get(*it, state_abs));
  }
  free(state_str);
  return h_value;
}


/* Heuristic with pdbs top spin. */
unsigned pdb_topspin(state_t *state) {
  state_t *state_abs = new state_t;
  unsigned h_value = 0;
  int index = 0;
  char *state_str = (char*) calloc(MAX_STATE_LEN, sizeof(char)); 
  char *state_abs_str;

  // Sumamos las heuristicas.
  for (vector<state_map_t*>::iterator it = pdbs.begin(); it != pdbs.end(); it++) {
    // Guardamos el estado en un string.
    sprint_state(state_str, MAX_STATE_LEN, state);

    // Creamos un estado a partir de la abstraccion.
    state_abs_str = make_state_abs_topspin(state_str, partition_topspin[index++]);
    read_state(state_abs_str, state_abs);
    free(state_abs_str);

    // Agregamos el valor del estado abstraido.
    h_value = f(h_value, (unsigned) *state_map_get(*it, state_abs));
  }
  free(state_str);
  return h_value;
}


/* Heuristic with pdbs rubiks cube. */
unsigned pdb_rubik(state_t *state) {
  state_t *state_abs = new state_t;
  unsigned h_value = 0;
  int index = 0;
  char *state_str = (char*) calloc(MAX_STATE_LEN, sizeof(char)); 
  char *state_abs_str;

  // Sumamos las heuristicas.
  for (vector<state_map_t*>::iterator it = pdbs.begin(); it != pdbs.end(); it++) {
    // Guardamos el estado en un string.
    sprint_state(state_str, MAX_STATE_LEN, state);

    // Creamos un estado a partir de la abstraccion.
    state_abs_str = make_state_abs_rubik(state_str, partition_rubik[index++]);
    read_state(state_abs_str, state_abs);
    free(state_abs_str);

    // Agregamos el valor del estado abstraido.
    h_value = f(h_value, (unsigned) *state_map_get(*it, state_abs));
  }
  cout << "VALUE: " << h_value << "\n";
  free(state_str);
  return h_value;
}
