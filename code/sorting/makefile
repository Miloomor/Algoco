# Uso:
#   make          -> compila el programa principal
#   make run      -> genera los arreglos de entrada y ejecuta las mediciones
#   make plots    -> genera los gráficos desde Python
#   make clean    -> elimina ejecutables y archivos de medición
#   make all      -> compila, genera datos y grafica

CXX      = g++
CXXFLAGS = -O2 -std=c++17 -Wall

TARGET   = sorting
SRC      = sorting.cpp

# Directorios de datos
INPUT_DIR   = data/array_input
OUTPUT_DIR  = data/array_output
MEAS_DIR    = data/measurements
PLOTS_DIR   = data/plots

# Scripts
ARRAY_GEN   = scripts/array_generator.py
PLOT_SCRIPT = scripts/plot_generator.py

.PHONY: all compile run plots clean dirs

## Objetivo por defecto: compilar
all: compile run plots

## Compilar el programa principal
compile: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)
	@echo "Compilación exitosa: ./$(TARGET)"

## Crear directorios necesarios
dirs:
	mkdir -p $(INPUT_DIR) $(OUTPUT_DIR) $(MEAS_DIR) $(PLOTS_DIR)

## Generar arreglos de entrada y ejecutar mediciones
run: compile dirs
	@echo "--- Generando arreglos de entrada ---"
	cd scripts && python3 array_generator.py
	@echo "--- Ejecutando mediciones ---"
	./$(TARGET)
	@echo "--- Mediciones completadas ---"

## Generar gráficos
plots: dirs
	@echo "--- Generando gráficos ---"
	python3 $(PLOT_SCRIPT)
	@echo "--- Gráficos guardados en $(PLOTS_DIR) ---"

## Limpiar
clean:
	rm -f $(TARGET)
	rm -f $(MEAS_DIR)/*.csv
	rm -f $(OUTPUT_DIR)/*.txt
	rm -f $(PLOTS_DIR)/*.png
	@echo "Limpieza completada"
