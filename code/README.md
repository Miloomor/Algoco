# Documentación

## Entrega

La entrega se realiza vía **aula.usm.cl** en formato `.zip`.

El `.zip` debe contener la siguiente estructura:

```
code/
├── matrix_multiplication/
│   ├── algorithms/
│   │   ├── naive.cpp
│   │   └── strassen.cpp
│   ├── data/
│   │   ├── matrix_input/
│   │   ├── matrix_output/
│   │   └── measurements/
│   └── matrix_multiplication.cpp
└── sorting/
    ├── algorithms/
    │   ├── mergesort.cpp
    │   ├── quicksort.cpp
    │   └── sort.cpp
    ├── data/
    │   ├── array_input/
    │   ├── array_output/
    │   └── measurements/
    └── sorting.cpp
```

---

## Multiplicación de matrices

Algoritmos implementados: **Naive** y **Strassen**.

### Programa principal

**Archivo:** `matrix_multiplication/matrix_multiplication.cpp`

Lee pares de matrices desde `data/matrix_input/`, ejecuta ambos algoritmos sobre cada par, y escribe los resultados en `data/matrix_output/` y las mediciones en `data/measurements/results.csv`.

**Parámetros de experimentación:**

| Parámetro | Valores |
|-----------|---------|
| Tamaño `n` | 16, 64, 256, 1024 |
| Tipo | `dispersa`, `diagonal`, `densa` |
| Dominio | `D0`, `D10` |
| Muestra | `a`, `b`, `c` |

**Formato de archivos de entrada:**

```
data/matrix_input/{n}_{tipo}_{dominio}_{muestra}_1.txt
data/matrix_input/{n}_{tipo}_{dominio}_{muestra}_2.txt
```

Cada archivo contiene una matriz de enteros separados por espacios, una fila por línea.

**Formato del CSV de mediciones** (`data/measurements/results.csv`):

```
algorithm,n,tipo,dominio,muestra,time_ms,memory_kb
```

- `time_ms`: tiempo de ejecución del algoritmo en milisegundos.
- `memory_kb`: pico de memoria dinámica utilizada por el algoritmo (en KB), medido mediante la sobreescritura de `operator new` / `operator delete`.

**Compilación y ejecución** (desde `matrix_multiplication/`):

```bash
g++ -O2 -o matrix_multiplication matrix_multiplication.cpp
./matrix_multiplication
```

### Scripts

Los scripts deben ejecutarse desde `matrix_multiplication/` y se encargan de generar los datos de entrada y graficar los resultados.

**Generación de matrices de entrada:**

```bash
# Genera todos los archivos de entrada en data/matrix_input/
python3 scripts/generate_matrices.py
```

**Visualización de resultados:**

```bash
# Genera gráficos a partir de data/measurements/results.csv
python3 scripts/plot_results.py
```

---

## Ordenamiento de arreglo unidimensional

Algoritmos implementados: **MergeSort**, **QuickSort** (3-way, Dutch National Flag) y **std::sort**.

### Programa principal

**Archivo:** `sorting/sorting.cpp`

Lee arreglos desde `data/array_input/`, ejecuta los tres algoritmos sobre cada arreglo, y escribe los resultados en `data/array_output/` y las mediciones en `data/measurements/results.csv`.

**Parámetros de experimentación:**

| Parámetro | Valores |
|-----------|---------|
| Tamaño `n` | 10, 1000, 100000, 10000000 |
| Tipo | `ascendente`, `descendente`, `aleatorio` |
| Dominio | `D1`, `D7` |
| Muestra | `a`, `b`, `c` |

**Formato de archivos de entrada:**

```
data/array_input/{n}_{tipo}_{dominio}_{muestra}.txt
```

Cada archivo contiene enteros separados por espacios en una sola línea.

**Formato del CSV de mediciones** (`data/measurements/results.csv`):

```
algorithm,n,tipo,dominio,muestra,time_ms,memory_kb
```

- `time_ms`: tiempo de ejecución en milisegundos.
- `memory_kb`: diferencia de `VmRSS` (memoria residente) antes y después de ejecutar el algoritmo, leída desde `/proc/self/status`.

**Compilación y ejecución** (desde `sorting/`):

```bash
g++ -O2 -o sorting sorting.cpp
./sorting
```

### Scripts

Los scripts deben ejecutarse desde `sorting/` y se encargan de generar los datos de entrada y graficar los resultados.

**Generación de arreglos de entrada:**

```bash
# Genera todos los archivos de entrada en data/array_input/
python3 scripts/generate_arrays.py
```

**Visualización de resultados:**

```bash
# Genera gráficos a partir de data/measurements/results.csv
python3 scripts/plot_results.py
```