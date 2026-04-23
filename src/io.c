#include <stdio.h>
#include <stdbool.h>
#include "../include/lga.h"

// Подсчет количества частиц в клетке по битовой маске
int count_particles(uint8_t state) {
    int count = 0;
    for (int i = 0; i < 7; i++) {
        if (state & (1 << i)) count++;
    }
    return count;
}

// Вывод решетки в терминал с использованием ANSI-цветов
void print_grid_to_console(Cell** grid, int width, int height) {
    for (int y = 0; y < height; y++) {
        // Сдвиг нечетных строк для визуализации гексагональной сетки
        if (y % 2 != 0) {
            printf(" ");
        }
        for (int x = 0; x < width; x++) {
            if (grid[y][x].is_obstacle) {
                printf("\033[34m# \033[0m"); // Синий цвет для препятствий
            } else if (grid[y][x].state == 0) {
                printf("  ");                // Пустая клетка
            } else {
                printf("\033[31m* \033[0m"); // Красный цвет для частиц
            }
        }
        printf("\n");
    }
}

// Сохранение текущего кадра в графический формат PPM
void save_ppm(Cell** grid, int width, int height, int frame) {
    char filename[64];
    sprintf(filename, "output/frame_%04d.ppm", frame);
    
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Ошибка открытия файла %s\n", filename);
        return;
    }
    
    // Заголовок PPM файла
    fprintf(f, "P3\n%d %d\n255\n", width, height);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x].is_obstacle) {
                fprintf(f, "0 0 0 "); // Черный цвет для препятствий
            } else {
                int p_count = count_particles(grid[y][x].state);
                if (p_count == 0) {
                    fprintf(f, "255 255 255 "); // Белый цвет для пустых клеток
                } else {
                    // Интенсивность синего цвета зависит от количества частиц
                    int color = 255 - (p_count * 35); 
                    if (color < 0) color = 0;
                    fprintf(f, "%d %d 255 ", color, color);
                }
            }
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

// Сохранение состояния решетки в бинарный файл
void save_bin(Cell** grid_a, int width, int height) {
    FILE *f_bin = fopen("grid.bin", "wb");
    for (int y = 0; y < height; y++) {
        fwrite(grid_a[y], sizeof(Cell), width, f_bin);
    }
    fclose(f_bin);
    printf("Состояние сохранено в grid.bin\n");
}

// Сохранение средней плотности в текстовый файл
void save_txt(double avg_rho) {
    FILE *f_out = fopen("result.txt", "w");
    if (f_out) {
        fprintf(f_out, "Средняя плотность частиц: %f\n", avg_rho);
        fclose(f_out);
    }
}