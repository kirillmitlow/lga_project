#include <stdlib.h>
#include <stdbool.h>
#include "../include/lga.h"

// Смещения для соседних клеток в гексагональной решетке.
// dx имеет два массива: для четных и нечетных строк.
int dy[6] = {0, -1, -1, 0, 1, 1};
int dx[2][6] = {
    {1, 0, -1, -1, -1, 0}, 
    {1, 1, 0, -1, 0, 1}    
};

// Глобальные таблицы поиска для двух хиральных состояний
uint8_t lut_a[128];
uint8_t lut_b[128];


// Инициализирует таблицы поиска (LUT) для обработки столкновений по правилам FHP-II
// Функция предрассчитывает результаты всех 128 возможных состояний клетки (6 направлений + 1 rest)
// Битовая маска: биты 0-5 отвечают за движущиеся частицы, бит 6 (0x40) — за покоящуюся (rest)
void init_lut(void) {
    for (int i = 0; i < 128; i++) {
        // По умолчанию состояние не меняется (стриминг без столкновений)
        lut_a[i] = i;
        lut_b[i] = i;
        
        uint8_t dirs = i & 0x3F; // Маска 00111111
        uint8_t rest = i & 0x40; // Маска 01000000
        
        // Двухчастичные лобовые столкновения.
        // Частицы разлетаются под углом 60 градусов.
        if (dirs == 9) {
            lut_a[i] = 18 | rest;
            lut_b[i] = 36 | rest;
        }
        else if (dirs == 18) {    // Направления 1 и 4
            lut_a[i] = 9 | rest;
            lut_b[i] = 36 | rest;
        }
        else if (dirs == 36) {    // Направления 2 и 5
            lut_a[i] = 9 | rest;
            lut_b[i] = 18 | rest;
        }
        
        // Трехчастичные симметричные столкновения (120 градусов)
        // Отражаются строго в противоположные стороны
        else if (dirs == 21) {   // Направления 0, 2, 4
            lut_a[i] = 42 | rest;
            lut_b[i] = 42 | rest;
        } 
        else if (dirs == 42) {   // Направления 1, 3, 5
            lut_a[i] = 21 | rest;
            lut_b[i] = 21 | rest;
        }

        // Взаимодействие с rest-частицей (сохранение импульса и массы).
        // Две движущиеся частицы + 1 пустая клетка -> 1 движущаяся + 1 rest.
        else if (dirs == 5 && rest == 0) {
            lut_a[i] = 2 | 0x40;
            lut_b[i] = 2 | 0x40;
        }
        else if (dirs == 10 && rest == 0) { 
            lut_a[i] = 4 | 0x40;
            lut_b[i] = 4 | 0x40;
        }
        else if (dirs == 20 && rest == 0) {
            lut_a[i] = 8 | 0x40;
            lut_b[i] = 8 | 0x40;
        }
        else if (dirs == 40 && rest == 0) {
            lut_a[i] = 16 | 0x40;
            lut_b[i] = 16 | 0x40;
        }
        else if (dirs == 17 && rest == 0) {
            lut_a[i] = 32 | 0x40;
            lut_b[i] = 32 | 0x40;
        }
        else if (dirs == 34 && rest == 0) {
            lut_a[i] = 1 | 0x40;
            lut_b[i] = 1 | 0x40;
        }

        // Обратный процесс: 1 движущаяся + 1 rest -> 2 движущиеся частицы.
        else if (dirs == 2 && rest == 0x40) {
            lut_a[i] = 5; 
            lut_b[i] = 5; 
        }
        else if (dirs == 4 && rest == 0x40) {
            lut_a[i] = 10;
            lut_b[i] = 10;
        }
        else if (dirs == 8 && rest == 0x40) {
            lut_a[i] = 20;
            lut_b[i] = 20;
        }
        else if (dirs == 16 && rest == 0x40) {
            lut_a[i] = 40;
            lut_b[i] = 40;
        }
        else if (dirs == 32 && rest == 0x40) { 
            lut_a[i] = 17;
            lut_b[i] = 17;
        }
        else if (dirs == 1 && rest == 0x40) { 
            lut_a[i] = 34;
            lut_b[i] = 34;
        }
    }
}

//Инициализирует решетку частицами со случайным распределением.
void init_particles(Cell** grid, int width, int height, double rho) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Препятствия не заполняются газом
            if (grid[y][x].is_obstacle == true) continue;
            
            // Пробегаемся по 6 направлениям движения
            for (int dir = 0; dir < 6; dir++) {
                double r = (double)rand() / RAND_MAX;
                // Вероятность появления частицы в одном направлении равна rho / 6
                if (r < (rho / 6.0)) {
                    grid[y][x].state |= (1 << dir); // Устанавливаем соответствующий бит
                }
            }
            // Примечание: инициализация rest-частиц здесь не предусмотрена, 
            // они появятся в процессе столкновений
        }
    }
}

// Размещает препятствия на решетке (стенки канала и центральный цилиндр).
void init_obstacles(Cell** grid, int width, int height, int diameter) {
    double r = diameter / 2.0;
    
    // Формируем верхнюю и нижнюю стенки канала (граничные условия bounce-back)
    for (int x = 0; x < width; x++) {
        grid[0][x].is_obstacle = true;
        grid[height-1][x].is_obstacle = true;
    }
    
    // Вычисляем координаты центра канала
    int cx = width / 2;
    int cy = height / 2;
    
    // Формируем круглое препятствие (цилиндр) в центре
    for(int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Уравнение круга: (x - cx)^2 + (y - cy)^2 <= R^2
            int a = (x - cx) * (x - cx);
            int b = (y - cy) * (y - cy);
            if (a + b <= r * r) {
                grid[y][x].is_obstacle = true;
                grid[y][x].state = 0;
            }
        }
    }
}

// Выделяет память под двумерную решетку клеток.
Cell** allocate_grid(int width, int height) {
    Cell** grid = (Cell**)calloc(height, sizeof(Cell*));
    for (int i = 0; i < height; i++) {
        grid[i] = (Cell*)calloc(width, sizeof(Cell));
    }
    return grid;
}