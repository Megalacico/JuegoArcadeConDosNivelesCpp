#pragma once

#include <string>
#include <vector>

class Spritezi {
private:
    std::string carpeta;
    std::vector<std::string> frames;
    int velocidad_ms;
    int repeticiones;
    bool orden_ascendente;

    void limpiarPantalla();
    void pintar(const std::vector<std::string>& frame);
    std::vector<std::string> cargar_frame(const std::string& fichero);
    std::vector<std::string> obtener_archivos(const std::string& carpeta);

public:
    Spritezi(const std::string& carpeta, int fps = 30, int repeticiones = 777, bool orden_ascendente = true);

    void reproducir();
};