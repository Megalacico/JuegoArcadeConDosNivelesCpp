#include "spritezi.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <thread>
#include <chrono>
#include <cstdlib>

namespace fs = std::filesystem;

Spritezi::Spritezi(const std::string& carpeta, int fps, int repeticiones, bool orden_ascendente)
    : carpeta(carpeta),
    velocidad_ms(1000 / fps),
    repeticiones(repeticiones),
    orden_ascendente(orden_ascendente)
{
    frames = obtener_archivos(carpeta);

    if (frames.empty()) {
        std::cerr << "carpeta no encontrada x_x  - > " << carpeta << std::endl;
    }
}

void Spritezi::limpiarPantalla() {
    system("cls");
}

void Spritezi::pintar(const std::vector<std::string>& frame) {
    for (const std::string& linea : frame) {
        std::cout << linea << std::endl;
    }
}

std::vector<std::string> Spritezi::cargar_frame(const std::string& fichero) {
    std::vector<std::string> frame;
    std::ifstream archivo(fichero);
    std::string linea;

    if (!archivo) {
        std::cerr << "Error: archivo no encontrado 404 x_x" << fichero << std::endl;
        return frame;
    }

    while (std::getline(archivo, linea)) {
        frame.push_back(linea);
    }

    archivo.close();
    return frame;
}

std::vector<std::string> Spritezi::obtener_archivos(const std::string& carpeta) {
    std::vector<std::string> rutas;

    for (const auto& entry : fs::directory_iterator(carpeta)) {
        if (entry.path().extension() == ".txt") {
            rutas.push_back(entry.path().string());
        }
    }

    std::sort(rutas.begin(), rutas.end(), [](const std::string& a, const std::string& b) {
        int numA = std::stoi(fs::path(a).stem().string());
        int numB = std::stoi(fs::path(b).stem().string());
        return numA < numB;
        });

    return rutas;
}

void Spritezi::reproducir() {
    if (frames.empty()) return;

    int contador = 0;
    while (repeticiones == 777 || contador < repeticiones) {
        if (orden_ascendente) {
            for (const std::string& f : frames) {
                limpiarPantalla();
                auto frame = cargar_frame(f);
                pintar(frame);
                std::this_thread::sleep_for(std::chrono::milliseconds(velocidad_ms));
            }
        }
        else {
            for (auto it = frames.rbegin(); it != frames.rend(); ++it) {
                limpiarPantalla();
                auto frame = cargar_frame(*it);
                pintar(frame);
                std::this_thread::sleep_for(std::chrono::milliseconds(velocidad_ms));
            }
        }
        contador++;
    }
}