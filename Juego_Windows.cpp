#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <Windows.h>
#include "spritezi.h"

using namespace std;


const int CELDA_VACIA = 0;
const int CELDA_PROYECTIL = 1;
const int CELDA_JUGADOR = 3;
const int CELDA_BORDE = 5;
const int CELDA_ENEMIGO = 7;
const int CELDA_DISPARO = 9;


int menu_del_juego() {
    int menu_selector;
    do {
        system("cls");
        cout << "========= MENU DEL JUEGO =========" << endl;
        cout << "    1.--Jugar--" << endl;
        cout << "    2.--Instrucciones--" << endl;
        cout << "    3.--Créditos--" << endl;
        cout << "    4.--Salir--" << endl;
        cout << "Ingrese una opción -> ";
        cin >> menu_selector;

        if (menu_selector == 1) {
            return 1;
        }
        else if (menu_selector == 2) {
            system("cls");
            cout << "========== INSTRUCCIONES ==========" << endl;
            cout << "1. Puedes moverte con [A] (izquierda) y [D] (derecha)." << endl;
            cout << "2. Para atrapar los objetivos, deben caer en la mano del personaje." << endl;
            cout << "3. Dispara con [W] y sal del juego con [Q]." << endl << endl;
            cout << "Alcanza las vacunas y gana puntos / Elimina los escombros y gana el doble de puntos" << endl << endl;
            system("pause");
        }
        else if (menu_selector == 3) {
            system("cls");
            cout << "=========== CRÉDITOS =============" << endl;
            cout << "- Juego creado en C++" << endl;
            system("pause");
        }
        else if (menu_selector == 4) {
            cout << "Saliendo del juego..." << endl;
            system("pause");
            return 4;
        }
    } while (true);
}


void mover_cursor_inicio() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { 0, 0 };
    SetConsoleCursorPosition(hConsole, pos);
}

bool es_colision(int celda) {
    return celda == CELDA_BORDE;
}

char calcular_calificacion(int& vacunas_perdidas, int& enemigos_destruidos) {
    if (vacunas_perdidas <= 10 && enemigos_destruidos > 10) return 'A';
    if (vacunas_perdidas <= 25 && enemigos_destruidos > 6) return 'B';
    if (vacunas_perdidas <= 40 && enemigos_destruidos > 4) return 'C';
    return 'D';
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    srand(time(0));

    int nivel = 1;
    int menu_result = menu_del_juego();
    if (menu_result == 4) return 0;
    Spritezi animacion("animacion1", 1000, 10, true); // Cargar animación
    animacion.reproducir(); // Reproducir animación al inicio
    while (true) {
        int meta_puntos = (nivel == 1) ? 10 : 20;
        int total_vacunas = (nivel == 1) ? 200 : 200;
        int total_enemigos = (nivel == 1) ? 0 : 50;
        int vida = 5;
        int vacunas_perdidas = 0;
        int enemigos_destruidos = 0;
        const int FILAS = 20;
        const int COLUMNAS = 30;
        const int DIFICULTAD_SPAWN = 200;
        const int DIFICULTAD_ENEMIGO = 400;

        vector<vector<int>> espacio(FILAS, vector<int>(COLUMNAS, CELDA_VACIA));
        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                if (i == 0 || i == FILAS - 1 || j == 0 || j == COLUMNAS - 1) {
                    espacio[i][j] = CELDA_BORDE;
                }
            }
        }


        int puntos = 0;
        int fila_jugador = FILAS - 2;
        int col_jugador = COLUMNAS / 2;
        int fila_disparo = -1, col_disparo = -1;

        for (int k = -2; k <= 2; k++) {
            if (col_jugador + k >= 1 && col_jugador + k < COLUMNAS - 1)
                espacio[fila_jugador][col_jugador + k] = CELDA_JUGADOR;
        }
        cout << endl;
        cout << "Nivel " << nivel << ": ";
        if (nivel == 1)
            cout << "¡Atrapa todas las vacunas!" << endl;
        else
            cout << "¡Atrapa vacunas y elimina los escombros!" << endl;
        this_thread::sleep_for(chrono::milliseconds(2000));
        system("cls");

        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        while (true) {

            if (_kbhit()) {
                char tecla = _getch();
                for (int k = -2; k <= 2; k++) {
                    if (col_jugador + k >= 1 && col_jugador + k < COLUMNAS - 1)
                        espacio[fila_jugador][col_jugador + k] = CELDA_VACIA;
                }

                if (tecla == 'a' && !es_colision(espacio[fila_jugador][col_jugador - 3])) {
                    col_jugador--;
                }
                else if (tecla == 'd' && !es_colision(espacio[fila_jugador][col_jugador + 4])) {
                    col_jugador++;
                }
                else if (tecla == 'w' && nivel > 1) {
                    if (fila_disparo == -1) {
                        fila_disparo = fila_jugador - 1;
                        col_disparo = col_jugador - 2;
                        if (espacio[fila_disparo][col_disparo] == CELDA_VACIA) {
                            espacio[fila_disparo][col_disparo] = CELDA_DISPARO;
                        }
                    }
                }
                else if (tecla == 'q') {
                    return 0;
                }

                for (int k = -2; k <= 2; ++k) {
                    if (col_jugador + k >= 1 && col_jugador + k < COLUMNAS - 1)
                        espacio[fila_jugador][col_jugador + k] = CELDA_JUGADOR;
                }
            }

            for (int fila = FILAS - 2; fila >= 0; fila--) {
                for (int col = 1; col < COLUMNAS - 1; col++) {
                    if (espacio[fila][col] == CELDA_PROYECTIL) {
                        espacio[fila][col] = CELDA_VACIA;
                        bool colisiona_jugador = false;
                        for (int k = -2; k <= 2; k++) {
                            if (col + k >= 1 && col + k < COLUMNAS - 1 && espacio[fila + 1][col + k] == CELDA_JUGADOR) {
                                colisiona_jugador = true;
                                break;
                            }
                        }
                        if (colisiona_jugador) {
                            puntos++;
                        }
                        else if (es_colision(espacio[fila + 1][col])) {
                            total_vacunas--;
                            vacunas_perdidas++;
                        }
                        else {
                            espacio[fila + 1][col] = CELDA_PROYECTIL;
                        }
                    }
                }
                if (nivel > 1) {
                    for (int col = 1; col < COLUMNAS - 3; col++) {
                        if (espacio[fila][col] == CELDA_ENEMIGO &&
                            espacio[fila][col + 1] == CELDA_ENEMIGO &&
                            espacio[fila][col + 2] == CELDA_ENEMIGO) {
                            espacio[fila][col] = CELDA_VACIA;
                            espacio[fila][col + 1] = CELDA_VACIA;
                            espacio[fila][col + 2] = CELDA_VACIA;
                            if (espacio[fila + 1][col] == CELDA_JUGADOR ||
                                espacio[fila + 1][col + 1] == CELDA_JUGADOR ||
                                espacio[fila + 1][col + 2] == CELDA_JUGADOR) {
                                vida--;
                            }
                            if (espacio[fila + 1][col] == CELDA_DISPARO ||
                                espacio[fila + 1][col + 1] == CELDA_DISPARO ||
                                espacio[fila + 1][col + 2] == CELDA_DISPARO) {
                                if (total_enemigos > 0) total_enemigos--;
                                puntos += 2;
                                enemigos_destruidos++;
                            }
                            else if (es_colision(espacio[fila + 1][col]) ||
                                es_colision(espacio[fila + 1][col + 1]) ||
                                es_colision(espacio[fila + 1][col + 2])) {
                                if (total_enemigos > 0) total_enemigos--;
                            }
                            else {
                                espacio[fila + 1][col] = CELDA_ENEMIGO;
                                espacio[fila + 1][col + 1] = CELDA_ENEMIGO;
                                espacio[fila + 1][col + 2] = CELDA_ENEMIGO;
                            }
                            col += 2;
                        }
                    }
                }
            }
            if (nivel > 1 && fila_disparo != -1) {
                if (espacio[fila_disparo][col_disparo] == CELDA_DISPARO)
                    espacio[fila_disparo][col_disparo] = CELDA_VACIA;
                fila_disparo--;
                if (fila_disparo <= 0 || es_colision(espacio[fila_disparo][col_disparo])) {
                    fila_disparo = -1;
                }
                else if (espacio[fila_disparo][col_disparo] == CELDA_ENEMIGO) {
                    int inicio = col_disparo;
                    while (inicio > 0 && espacio[fila_disparo][inicio - 1] == CELDA_ENEMIGO)
                        inicio--;
                    for (int k = 0; k < 3; ++k) {
                        if (inicio + k < COLUMNAS && espacio[fila_disparo][inicio + k] == CELDA_ENEMIGO)
                            espacio[fila_disparo][inicio + k] = CELDA_VACIA;
                    }
                    if (total_enemigos > 0)
                        total_enemigos--;
                    puntos += 2;
                    fila_disparo = -1;
                    enemigos_destruidos++;
                }
                else {
                    espacio[fila_disparo][col_disparo] = CELDA_DISPARO;
                }
            }

            for (int col = 1; col < COLUMNAS - 1; col++) {
                if (espacio[1][col] == CELDA_VACIA) {
                    if (rand() % DIFICULTAD_SPAWN == 0) {
                        espacio[1][col] = CELDA_PROYECTIL;
                    }
                }
            }
            if (nivel > 1) {
                for (int col = 1; col < COLUMNAS - 3; col++) {
                    if (total_enemigos > 0 &&
                        espacio[1][col] == CELDA_VACIA &&
                        espacio[1][col + 1] == CELDA_VACIA &&
                        espacio[1][col + 2] == CELDA_VACIA) {
                        if (rand() % DIFICULTAD_ENEMIGO == 0) {
                            espacio[1][col] = CELDA_ENEMIGO;
                            espacio[1][col + 1] = CELDA_ENEMIGO;
                            espacio[1][col + 2] = CELDA_ENEMIGO;
                        }
                    }
                }
            }

            mover_cursor_inicio();
            cout << "Nivel: " << nivel << "  Puntos: " << puntos << " / " << meta_puntos << "    " << endl;
            cout << "Vida: " << vida << "    " << endl;
            cout << "Proyectiles restantes: " << total_vacunas << "    " << endl;
            cout << "Vacunas perdidas: " << vacunas_perdidas << "    " << endl;
            cout << "Escombros destruidos: " << enemigos_destruidos << "    " << endl;
            if (nivel > 1)
                cout << "Escombros restantes: " << total_enemigos << "    " << endl;
            cout << "Controles: 'a' (izquierda), 'd' (derecha)";
            if (nivel > 1) cout << ", 'w' (disparar)";
            cout << ", 'q' (salir)" << endl;

            for (int i = 0; i < FILAS; ++i) {
                for (int j = 0; j < COLUMNAS; ++j) {
                    switch (espacio[i][j]) {
                    case CELDA_VACIA:
                        cout << " ";
                        break;
                    case CELDA_PROYECTIL:
                        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                        cout << "▼";
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                        break;
                    case CELDA_ENEMIGO:
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
                        if (j < COLUMNAS - 3) {
                            cout << "▲▲▲";
                            j += 2;
                        }
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                        break;
                    case CELDA_JUGADOR:
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                        if (j <= COLUMNAS - 6 &&
                            espacio[i][j] == CELDA_JUGADOR &&
                            espacio[i][j + 1] == CELDA_JUGADOR &&
                            espacio[i][j + 2] == CELDA_JUGADOR &&
                            espacio[i][j + 3] == CELDA_JUGADOR &&
                            espacio[i][j + 4] == CELDA_JUGADOR) {
                            cout << "¯|(ツ)";
                            j += 5;
                            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                        }
                        break;
                    case CELDA_BORDE:
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                        cout << "∎";
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                        break;
                    case CELDA_DISPARO:
                        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                        cout << "^";
                        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                        break;
                    }
                }
                cout << endl;
            }

            if (puntos >= meta_puntos) {
                cout << "\n¡FELICIDADES! ¡Has alcanzado la meta de " << meta_puntos << " puntos!" << endl;
                if (puntos >= meta_puntos && nivel == 1) {
                    cout << "\n===== CALIFICACIÓN DEL JUGADOR =====" << endl;
                    cout << "Vacunas perdidas: " << vacunas_perdidas << endl;
                    cout << "Calificación: ";
                    if (vacunas_perdidas == 0) cout << "S - no fallaste nada, Perfecto";
                    else if (vacunas_perdidas <= 15) cout << "A - Fallaste menos o 15 vacunas, buen trabajo";
                    else if (vacunas_perdidas <= 30) cout << "B - Fallaste menos o 30 vacunas, puedes mejorar";
                    else if (vacunas_perdidas <= 50) cout << "C - Fallaste menos o 50 vacunas, Fallaste muchas";
                    else cout << "D - debes hacerlo mejor";
                    cout << endl << "====================================" << endl;
                    system("pause");
                    break;

                }
                if (puntos >= meta_puntos && nivel == 2) {
                    cout << "\n¡FELICIDADES! ¡Has alcanzado la meta de " << meta_puntos << " puntos!" << endl;
                    cout << "\n===== CALIFICACIÓN DEL JUGADOR =====" << endl;
                    cout << "Vacunas perdidas: " << vacunas_perdidas << endl;
                    cout << "Escombros destruidos: " << enemigos_destruidos << endl;
                    cout << "Calificación: ";
                    char calificacion = calcular_calificacion(vacunas_perdidas, enemigos_destruidos);
                    if (calificacion == 'A') cout << "A - Excelente";
                    else if (calificacion == 'B') cout << "B - Buen trabajo";
                    else if (calificacion == 'C') cout << "C - Puedes mejorar";
                    else cout << "D - Debes hacerlo mejor";
                    cout << endl << "====================================" << endl;
                }
                system("pause");
                break;
            }
            if (total_vacunas <= 0) {
                cout << "\nGAME OVER. Te has quedado sin vacunas." << endl;
                system("pause");
                return 0;
            }
            if (nivel > 1 && vida <= 0) {
                cout << "\nGAME OVER. Te han alcanzado los enemigos." << endl;
                system("pause");
                return 0;
            }
            this_thread::sleep_for(chrono::milliseconds(100));

        }

        if (nivel == 1) {
            nivel = 2;
            system("cls");
            cout << "¡Nivel 2! Ahora caen escombros y puedes disparar (tecla 'w')." << endl;
            system("pause");
        }
        else {
            break;
        }
    }

    return 0;
}