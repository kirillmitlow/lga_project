#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/lga.h"

int count_particles(uint8_t state) {
    int count = 0;
    for (int i = 0; i < 7; i++) {
        if (state & (1 << i)) count++;
    }
    return count;
}

void print_grid_to_console(Cell** grid, int width, int height) {
    for (int y = 0; y < height; y++) {
        if (y % 2 != 0) {
            printf(" ");
        }
        for (int x = 0; x < width; x++) {
            if (grid[y][x].is_obstacle) {
                printf("\033[34m# \033[0m");
            } else if (grid[y][x].state == 0) {
                printf("  ");
            } else {
                printf("\033[31m* \033[0m");
            }
        }
        printf("\n");
    }
}




void save_ppm(Cell** grid, int width, int height, int frame) {
    char filename[64];
    sprintf(filename, "output/frame_%04d.ppm", frame);
    
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Ошибка открытия файла %s\n", filename);
        return;
    }
    
    fprintf(f, "P3\n%d %d\n255\n", width, height);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x].is_obstacle) {
                fprintf(f, "0 0 0 ");
            } else {
                int p_count = count_particles(grid[y][x].state);
                if (p_count == 0) {
                    fprintf(f, "255 255 255 ");
                } else {
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



void save_bin(Cell** grid_a, int width, int height) {
    FILE *f_bin = fopen("grid.bin", "wb");
    for (int y = 0; y < height; y++) {
        fwrite(grid_a[y], sizeof(Cell), width, f_bin);
    }
    fclose(f_bin);
    printf("Состояние сохранено в grid.bin\n");
}

void save_txt(double avg_rho) {
    FILE *f_out = fopen("result.txt", "w");
    if (f_out) {
        fprintf(f_out, "Средняя плотность частиц: %f\n", avg_rho);
        fclose(f_out);
    }
}