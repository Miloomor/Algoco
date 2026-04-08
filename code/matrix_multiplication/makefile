# Uso:
#   make          -> compila el programa principal
#   make run      -> genera las matrices de entrada y ejecuta las mediciones
#   make plots    -> genera los gráficos desde Python
#   make clean    -> elimina ejecutables y archivos de medición
#   make all      -> compila, genera datos y grafica

CXX      = g++
CXXFLAGS = -O2 -std=c++17 -Wall

TARGET   = matrix_multiplication
SRC      = matrix_multiplication.cpp

# Directorios
INPUT_DIR   = data/matrix_input
OUTPUT_DIR  = data/matrix_output
MEAS_DIR    = data/measurements
PLOTS_DIR   = data/plots

# Scripts
MATRIX_GEN  = scripts/matrix_generator.py
PLOT_SCRIPT = scripts/plot_generator.py

.PHONY: all compile run plots clean dirs

## Objetivo por defecto: compilar
all: compile run plots

## Compilar
compile: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)
	@echo "Compilación exitosa: ./$(TARGET)"

## Crear directorios
dirs:
	mkdir -p $(INPUT_DIR) $(OUTPUT_DIR) $(MEAS_DIR) $(PLOTS_DIR)

## Generar matrices de entrada y ejecutar mediciones
run: compile dirs
	@echo "--- Generando matrices de entrada ---"
	cd scripts && python3 matrix_generator.py
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
