#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "../include/lga.h"

int dy[6] = {0, -1, -1, 0, 1, 1};
int dx[2][6] = {
    {1, 0, -1, -1, -1, 0}, 
    {1, 1, 0, -1, 0, 1}    
};

uint8_t lut_a[128];
uint8_t lut_b[128];



void init_lut(void) {
    for (int i = 0; i < 128; i++) {
        lut_a[i] = i;
        lut_b[i] = i;
        
        uint8_t dirs = i & 0x3F; 
        uint8_t rest = i & 0x40; 
        
        if (dirs == 9) {
            lut_a[i] = 18 | rest;
            lut_b[i] = 36 | rest;
        }
        else if (dirs == 18) {
            lut_a[i] = 9 | rest;
            lut_b[i] = 36 | rest;
        }
        else if (dirs == 36) {
            lut_a[i] = 9 | rest;
            lut_b[i] = 18 | rest;
        }
        else if (dirs == 21) {
            lut_a[i] = 42 | rest;
            lut_b[i] = 42 | rest;
        } 
        else if (dirs == 42) {
            lut_a[i] = 21 | rest;
            lut_b[i] = 21 | rest;
        }



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



void init_particles(Cell** grid, int width, int height, double rho) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x].is_obstacle == true) continue;
            for (int dir = 0; dir < 6; dir++) {
                double r = (double)rand() / RAND_MAX;
                if (r < (rho / 6.0)) {
                    grid[y][x].state |= (1 << dir);
                }
            }
        }
    }
}





void init_obstacles(Cell** grid, int width, int height, int diameter) {
    double r = diameter / 2.0;
    for (int x = 0; x < width; x++) {
        grid[0][x].is_obstacle = true;
        grid[height-1][x].is_obstacle = true;
    }
    int cx = width/2;
    int cy = height/2;
    
    for(int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int a = (x - cx) * (x - cx);
            int b = (y - cy) * (y - cy);
            if (a + b <= r * r) {
                grid[y][x].is_obstacle = true;
            }
        }
    }
}





Cell** allocate_grid(int width, int height) {
    Cell** grid = (Cell**)calloc(height, sizeof(Cell*));
    for (int i = 0; i < height; i++) {
        grid[i] = (Cell*)calloc(width, sizeof(Cell));
    }
    return grid;
}
