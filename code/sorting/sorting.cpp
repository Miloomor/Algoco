#include "algorithms/mergesort.cpp"
#include "algorithms/quicksort.cpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <functional>
#include <atomic>
#include <new>

// ─── Memory tracking ─────────────────────────────────────────────────────────
// Referencias para tracking de memoria:
// https://en.cppreference.com/w/cpp/memory/new/operator_new
// https://en.cppreference.com/w/cpp/memory/new/operator_delete

static std::atomic<long> currentMem{0};
static std::atomic<long> peakMem{0};
static std::atomic<bool> trackingEnabled{false};

void* operator new(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) throw std::bad_alloc();
    if (trackingEnabled) {
        long current = currentMem.fetch_add((long)size) + (long)size;
        long peak = peakMem.load();
        while (current > peak && !peakMem.compare_exchange_weak(peak, current));
    }
    return ptr;
}

void operator delete(void* ptr, size_t size) noexcept {
    if (trackingEnabled) currentMem.fetch_sub((long)size);
    free(ptr);
}

void operator delete(void* ptr) noexcept {
    free(ptr);
}

// Llama esto antes de cada algoritmo para resetear contadores
void resetMemTracking() {
    currentMem = 0;
    peakMem    = 0;
}

// Devuelve el pico de memoria en KB registrado desde el último reset
long getPeakMemKB() {
    return peakMem.load() / 1024;
}
 
static std::vector<int> readArray(const std::string& filepath) {
    std::FILE* fp = std::fopen(filepath.c_str(), "r");
    if (!fp) return {};
    std::vector<int> arr;
    arr.reserve(10000000);
    int x;
    while (std::fscanf(fp, "%d", &x) == 1) arr.push_back(x);
    std::fclose(fp);
    arr.shrink_to_fit();
    return arr;
}
 
static void writeArray(const std::string& filepath, const std::vector<int>& arr) {
    std::FILE* fp = std::fopen(filepath.c_str(), "w");
    if (!fp) return;
    for (size_t i = 0; i < arr.size(); i++) {
        std::fprintf(fp, "%d", arr[i]);
        if (i + 1 < arr.size()) std::fputc(' ', fp);
    }
    std::fputc('\n', fp);
    std::fclose(fp);
}
// ─── Main ────────────────────────────────────────────────────────────────────

int main() {
std::srand((unsigned)std::time(nullptr));

// Directorios relativos al ejecutable (se ejecuta desde code/sorting/)
const std::string inputDir  = "data/array_input/";
const std::string outputDir = "data/array_output/";
const std::string measDir   = "data/measurements/";

std::filesystem::create_directories(outputDir);
std::filesystem::create_directories(measDir);

// Parámetros del enunciado
const std::vector<int>         Ns = {10, 1000, 100000, 10000000};
const std::vector<std::string> Ts = {"ascendente", "descendente", "aleatorio"};
const std::vector<std::string> Ds = {"D1", "D7"};
const std::vector<std::string> Ms = {"a", "b", "c"};

// Algoritmos a medir: nombre + función wrapper
struct Algorithm {
    std::string name;
    std::function<void(std::vector<int>&)> sort;
};
std::vector<Algorithm> algorithms = {
    {"mergesort", [](std::vector<int>& a){ mergeSort(a, 0, (int)a.size()-1); }},
    {"quicksort", [](std::vector<int>& a){ quickSort(a, 0, (int)a.size()-1); }},
    {"stdsort",   [](std::vector<int>& a){ std::sort(a.begin(), a.end()); }}
};

// Archivo de mediciones: una fila por experimento
std::ofstream measFile(measDir + "results.csv");
measFile << "algorithm,n,tipo,dominio,muestra,time_ms,memory_kb\n";

int total = 0, done = 0;
total = (int)(Ns.size() * Ts.size() * Ds.size() * Ms.size() * algorithms.size());

for (int n : Ns) {
    for (const auto& t : Ts) {
        for (const auto& d : Ds) {
            for (const auto& m : Ms) {
                std::string filename = std::to_string(n) + "_" + t + "_" + d + "_" + m + ".txt";
                std::string inputPath = inputDir + filename;

                // Verificar que el archivo existe
                if (!std::filesystem::exists(inputPath)) {
                    std::cerr << "[SKIP] No encontrado: " << inputPath << "\n";
                    done += (int)algorithms.size();
                    continue;
                }

                for (const auto& algo : algorithms) {
                    std::vector<int> arr = readArray(inputPath);
                    if (arr.empty()) {
                        std::cerr << "[SKIP] Arreglo vacío: " << inputPath << "\n";
                        done++;
                        continue;
                    }

                    // Activar tracking y resetear contadores
                    resetMemTracking();
                    trackingEnabled = true;

                    // Medición de tiempo
                    auto start = std::chrono::high_resolution_clock::now();
                    algo.sort(arr);
                    auto end   = std::chrono::high_resolution_clock::now();

                    // Desactivar tracking antes de cualquier otra cosa
                    trackingEnabled = false;

                    double time_ms = std::chrono::duration<double, std::milli>(end - start).count();
                    long   mem_kb  = getPeakMemKB();

                    // Guardar arreglo ordenado (solo para el primer algoritmo para no triplicar datos)
                    if (algo.name == "mergesort") {
                        std::string outName = std::to_string(n) + "_" + t + "_" + d + "_" + m + "_out.txt";
                        writeArray(outputDir + outName, arr);
                    }

                    // Registrar medición
                    measFile << algo.name << ","
                                << n << ","
                                << t << ","
                                << d << ","
                                << m << ","
                                << time_ms << ","
                                << mem_kb  << "\n";

                    done++;
                    std::cout << "[" << done << "/" << total << "] "
                                << algo.name << " n=" << n
                                << " t=" << t << " d=" << d << " m=" << m
                                << " -> " << time_ms << " ms, " << mem_kb << " KB\n";
                }
            }
        }
    }
}

measFile.close();
std::cout << "\nMediciones guardadas en " << measDir << "results.csv\n";
return 0;
}