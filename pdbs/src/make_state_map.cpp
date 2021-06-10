#include <iostream>
#include <cstring>

using namespace std;

void add_state(state_map_t *state_map, char *entry) {
  // Obtenemos la representacion del estado en char*.
  char s[100];
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

int main(int argc, char **argv) {
  FILE *f;
  if ((f = fopen(argv[1], "r")) == NULL) {
    cout << "No se pudo el archivo '\e[1m" << argv[1] << "\e[0m'.\n";
    exit(1);
  };
  state_map_t *state_map = new_state_map();

  char *line = NULL;
  size_t len = 0;

  // Leemos linea por linea y cada entrada la agregamos al map.
  while (getline(&line, &len, f) != -1) {
    add_state(state_map, line);
  }
  fclose(f);

  // Guardamos el state_map.
  f = fopen(argv[2], "w");
  write_state_map(f, state_map);
  fclose(f);

  return 0;
}