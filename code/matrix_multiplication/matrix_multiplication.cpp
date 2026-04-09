#include "algorithms/naive.cpp"
#include "algorithms/strassen.cpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <filesystem>
#include <cmath>
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

// ─── I/O ─────────────────────────────────────────────────────────────────────

std::vector<std::vector<int>> readMatrix(const std::string& path) {
    std::ifstream file(path);
    std::vector<std::vector<int>> M;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::vector<int> row;
        int x;
        while (iss >> x) row.push_back(x);
        if (!row.empty()) M.push_back(row);
    }
    return M;
}

void writeMatrix(const std::string& path, const std::vector<std::vector<int>>& M) {
    std::ofstream file(path);
    for (const auto& row : M) {
        for (size_t j = 0; j < row.size(); j++) {
            file << row[j];
            if (j + 1 < row.size()) file << " ";
        }
        file << "\n";
    }
}

// ─── Main ────────────────────────────────────────────────────────────────────

int main() {
    const std::string inputDir  = "data/matrix_input/";
    const std::string outputDir = "data/matrix_output/";
    const std::string measDir   = "data/measurements/";

    std::filesystem::create_directories(outputDir);
    std::filesystem::create_directories(measDir);

    const std::vector<int>         Ns = {16, 64, 256, 1024};
    const std::vector<std::string> Ts = {"dispersa", "diagonal", "densa"};
    const std::vector<std::string> Ds = {"D0", "D10"};
    const std::vector<std::string> Ms = {"a", "b", "c"};

    struct Algorithm {
        std::string name;
        std::function<std::vector<std::vector<int>>(
            std::vector<std::vector<int>>,
            std::vector<std::vector<int>>)> multiply;
    };
    std::vector<Algorithm> algorithms = {
        {"naive",    [](auto A, auto B){ return naiveMultiply(A, B); }},
        {"strassen", [](auto A, auto B){ return strassenMultiply(A, B); }}
    };

    std::ofstream measFile(measDir + "results.csv");
    measFile << "algorithm,n,tipo,dominio,muestra,time_ms,memory_kb\n";

    int total = (int)(Ns.size() * Ts.size() * Ds.size() * Ms.size() * algorithms.size());
    int done  = 0;

    for (int n : Ns) {
        for (const auto& t : Ts) {
            for (const auto& d : Ds) {
                for (const auto& m : Ms) {
                    std::string base  = std::to_string(n) + "_" + t + "_" + d + "_" + m;
                    std::string path1 = inputDir + base + "_1.txt";
                    std::string path2 = inputDir + base + "_2.txt";

                    if (!std::filesystem::exists(path1) || !std::filesystem::exists(path2)) {
                        std::cerr << "[SKIP] No encontrado: " << base << "\n";
                        done += (int)algorithms.size();
                        continue;
                    }

                    // Leer matrices ANTES de activar el tracking para no
                    // contaminar la medición con allocations de I/O
                    auto M1 = readMatrix(path1);
                    auto M2 = readMatrix(path2);

                    if (M1.empty() || M2.empty()) {
                        std::cerr << "[SKIP] Matriz vacía: " << base << "\n";
                        done += (int)algorithms.size();
                        continue;
                    }

                    for (const auto& algo : algorithms) {

                        // Activar tracking y resetear contadores
                        resetMemTracking();
                        trackingEnabled = true;

                        auto t0 = std::chrono::high_resolution_clock::now();
                        auto C  = algo.multiply(M1, M2);
                        auto t1 = std::chrono::high_resolution_clock::now();

                        // Desactivar tracking antes de cualquier otra cosa
                        trackingEnabled = false;

                        double time_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
                        long   mem_kb  = getPeakMemKB();

                        if (algo.name == "naive") {
                            writeMatrix(outputDir + base + "_out.txt", C);
                        }

                        measFile << algo.name << ","
                                 << n         << ","
                                 << t         << ","
                                 << d         << ","
                                 << m         << ","
                                 << time_ms   << ","
                                 << mem_kb    << "\n";

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