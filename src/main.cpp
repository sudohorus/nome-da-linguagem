#include "Interpreter.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>
#include "Error.h"

int main(int argc, char** argv) {
    std::string path = (argc > 1) ? argv[1] : std::string("programs/program.txt");
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Erro ao abrir arquivo: " << path << "\n";
        return 1;
    }

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Interpreter interp;
    auto start = std::chrono::high_resolution_clock::now();

    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        Err::setCurrentLine(lineNumber);
        if (interp.isReadStatement(line)) {
            auto pause = std::chrono::high_resolution_clock::now();
            if (!interp.execute(line)) break;
            auto resume = std::chrono::high_resolution_clock::now();
            start += (resume - pause);
        } else {
            if (!interp.execute(line)) break;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    if (interp.isTimeExecEnabled()) {
        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        std::cout << std::fixed << std::setprecision(3) << "[timeexec] " << ms << " ms\n";
    }

    return 0;
}
