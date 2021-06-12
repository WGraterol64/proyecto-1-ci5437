# **Generador de PDBs**

Para generar un PDB de un puzzle, digamos `P`, primero debemos asegurarnos de que

1. Se encuentran los archivos fuentes
    *   `../psvn/bin/psvn2c`
    *   `../psvn/src/psvn2c_core.c` 
    *   `../psvn/src/psvn2c_state_map.c` 
    *   `../psvn/src/psvn2c_abstraction.c`
    *   `../psvn/src/psvnc.cpp`
    *   `../psvn/src/abstractor.c`
    *   `../psvn/global/dist.cpp`
    *   `./src/make_state_map.cpp`
2. Hay un archivo `P.psvn` en el directorio `../puzzles/`
3. El archivo `./src/abstraction` contiene una abstraccion valida para el puzzle
tal que cada linea puede ser
    *   `map DOMAIN VALUE1 VALUE2` para mapear el valor `VALUE1` del domino `DOMAIN`
    al valor `VALUE2`. Ambos valores deben pertenecer al domino.
    *   `project N` para eliminar el `N`-esimo estado del puzzle.

Asi, podemos ejecutar

```bash
make P
```

Y se generara un archivo `P.state_map` correspondiente al PDB de la abstraccion.
Se recomienda crear un directorio `./P_pdbs` para almacenar los PDB para las 
distintas abstracciones del puzzle, e identificarlas por bloque, por ejemplo
`P.1.state_map`. Es importante el orden alfabetico de los `.state_map` pues en 
ese orden seran cargados para correr las heuristicas.

Los archivos `.state_map` no son agregados al repositorio ya que pueden llegar a ser
muy pesados.

## **Ejemplos del archivo `abstraction`**

Hay varios ejemplos en el directorio `./src`.

### **15Puzzle**

Podemos colocar comentarios en el archivo para que esa regla de abstraccion 
no sea tomada en cuenta.

```
# map TILE 1 B
# map TILE 2 B
map TILE 3 B
map TILE 4 B
# map TILE 5 B
# map TILE 6 B
# map TILE 7 B
map TILE 8 B
map TILE 9 B
map TILE 10 B
map TILE 11 B
map TILE 12 B
map TILE 13 B
map TILE 14 B
map TILE 15 B
```

### **Cubo de Rubik**

Se pueden colocar tanto mapeos como proyecciones en el archivo al mismo tiempo.
En el siguiente ejemplo los colores `YELLOW`, `ORANGE` y `WHITE` al color `BLACK`, 
siendo indistinguibles entre si. Ademas, son eliminadas las posiciones pares del estado.

```
map COLOUR YELLOW BLACK
map COLOUR ORANGE BLACK
map COLOUR WHITE BLACK
project 1
project 3
project 5
project 7
project 9
project 11
project 13
project 15
project 17
project 19
project 21
project 23
project 25
project 27
project 29
project 31
project 33
project 35
project 37
project 39
project 41
project 43
project 45
project 47
```