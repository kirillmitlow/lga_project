#include <stdlib.h>
#include "../include/lga.h"

// Перемещение частиц (streaming) в соседние узлы
void streaming(Cell** grid_in, Cell** grid_out, int width, int height) {
    // Очищаем выходную решетку перед записью (кроме препятствий)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid_out[y][x].is_obstacle == false) {
                grid_out[y][x].state = 0;
            }
        }
    }
    
    // Перенос частиц из grid_in в grid_out
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid_in[y][x].is_obstacle == true) {
                continue; // Препятствия не обрабатываем
            }
            
            uint8_t state = grid_in[y][x].state;
            
            // Обрабатываем 6 направлений движения
            for (int dir = 0; dir < 6; dir++) {
                if (state & (1 << dir)) {
                    int row_type = y % 2;
                    int nx = x + dx[row_type][dir];
                    int ny = y + dy[dir];
                    
                    // Верхняя и нижняя границы: отражение (bounce-back)
                    if (ny < 0 || ny >= height) {
                        int reverse_dir = (dir + 3) % 6;
                        grid_out[y][x].state |= (1 << reverse_dir);
                    } 
                    // Левая и правая границы: свободный выход (outflow)
                    else if (nx < 0 || nx >= width) {
                        continue; // Частица уходит за пределы канала
                    } 
                    // Обработка столкновения с препятствием внутри канала
                    else {
                        if (grid_out[ny][nx].is_obstacle == false) {
                            grid_out[ny][nx].state |= (1 << dir); // Успешный шаг
                        } else {
                            // Отражение от препятствия
                            int reverse_dir = (dir + 3) % 6;
                            grid_out[y][x].state |= (1 << reverse_dir);
                        }
                    }
                }
            }
            
            // Покоящаяся частица (rest) остается на месте
            if (state & 0x40) {
                grid_out[y][x].state |= 0x40;
            }
        }
    }
}

// Генерация частиц на входе (левая граница канала)
void apply_inlet(Cell** grid, int width, int height, double rho_in) {
    (void)width; // Игнорируем предупреждение о неиспользуемой переменной
    
    // Вероятность генерации частицы для каждого из 3 правых направлений
    double prob = rho_in / 3.0; 
    int right_dirs[3] = {0, 1, 5}; // Направления, смотрящие вправо
    
    for (int y = 0; y < height; y++) {
        if (grid[y][0].is_obstacle) continue;

        // Случайным образом добавляем новые частицы
        for (int i = 0; i < 3; i++) {
            int dir = right_dirs[i];
            double r = (double)rand() / RAND_MAX;
            if (r < prob) {
                grid[y][0].state |= (1 << dir);
            }
        }
    }
}