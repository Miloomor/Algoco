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

long getMemoryKB() {
std::ifstream f("/proc/self/status");
std::string line;
while (std::getline(f, line)) {
    if (line.rfind("VmRSS:", 0) == 0) {
        std::istringstream iss(line);
        std::string key; long val; iss >> key >> val;
        return val;
    }
}
return -1;
}

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

// Parámetros del enunciado (N = {2^4, 2^6, 2^8, 2^10})
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

                auto M1 = readMatrix(path1);
                auto M2 = readMatrix(path2);

                if (M1.empty() || M2.empty()) {
                    std::cerr << "[SKIP] Matriz vacía: " << base << "\n";
                    done += (int)algorithms.size();
                    continue;
                }

                for (const auto& algo : algorithms) {
                    // Strassen para n=1024 puede ser muy lento; saltarlo opcionalmente
                    // Se incluye de todas formas para completitud del experimento.

                    long memBefore = getMemoryKB();

                    auto t0 = std::chrono::high_resolution_clock::now();
                    auto C  = algo.multiply(M1, M2);
                    auto t1 = std::chrono::high_resolution_clock::now();

                    long memAfter = getMemoryKB();

                    double time_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
                    long   mem_kb  = memAfter - memBefore;

                    // Guardar resultado (solo naive para no duplicar)
                    if (algo.name == "naive") {
                        writeMatrix(outputDir + base + "_out.txt", C);
                    }

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
                                << " -> " << time_ms << " ms\n";
                }
            }
        }
    }
}

measFile.close();
std::cout << "\nMediciones guardadas en " << measDir << "results.csv\n";
return 0;
}