#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "../include/lga.h"


int main(int argc, char *argv[]) {
    int width = 60;
    int height = 21;
    int iters = 100;
    double rho_in = 0.8;
    int diameter = 6;
    int seed = time(NULL);
    int viz_interval = 10;

    int opt;
    while ((opt = getopt(argc, argv, "w:h:i:r:d:s:v:")) != -1) {
        switch (opt) {
            case 'w':
                width = atoi(optarg);
                break;
            case 'h':
                height = atoi(optarg);
                break;
            case 'i':
                iters = atoi(optarg);
                break;
            case 'r':
                rho_in = atof(optarg);
                break;
            case 'd':
                diameter = atoi(optarg);
                break;
            case 's':
                seed = atoi(optarg);
                break;
            case 'v':
                viz_interval = atoi(optarg);
                break;
            case '?':
                printf("Использование: %s -w <ширина> -h <высота> -i <итерации> -r <плотность> -d <диаметр> -s <seed> -v <viz_interval>\n", argv[0]);
                return 1;
        }
    }
    srand(seed);
    init_lut(); 

    Cell** grid_a = allocate_grid(width, height);
    Cell** grid_b = allocate_grid(width, height);

    init_obstacles(grid_a, width, height, diameter);
    init_obstacles(grid_b, width, height, diameter);

    init_particles(grid_a, width, height, rho_in);

    for (int i = 0; i < iters; i++) {
        //вывод в консоль
        //printf("\033[H\033[J");
        //print_grid_to_console(grid_a, width, height);
        
        streaming(grid_a, grid_b, width, height);
        apply_inlet(grid_b, width, height, rho_in);
        collision(grid_b, width, height);
        if (i % viz_interval == 0) {
            save_ppm(grid_a, width, height, i / viz_interval);
        }
        usleep(50000);

        Cell** temp_b = grid_b;
        grid_b = grid_a;
        grid_a = temp_b;
    }


    long total_particles = 0;
    long free_cells = 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (!grid_a[y][x].is_obstacle) {
                free_cells++;
                for (int bit = 0; bit < 7; bit++) {
                    if (grid_a[y][x].state & (1 << bit)) {
                        total_particles++;
                    }
                }
            }
        }
    }
    printf("Симуляция завершена.\n");

    double avg_rho = (double)total_particles / free_cells;

    save_txt(avg_rho);
    // сохранение в бинарный файл grid.bin
    save_bin(grid_a, width, height);
    return 0;
}