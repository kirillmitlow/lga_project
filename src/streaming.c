#include "../include/lga.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

void streaming(Cell** grid_in, Cell** grid_out, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid_out[y][x].is_obstacle == false) {
                grid_out[y][x].state = 0;
            }
        }
    }
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid_in[y][x].is_obstacle == true) {
                continue;
            }
            
            uint8_t state = grid_in[y][x].state;
            
            for (int dir = 0; dir < 6; dir++) {
                if (state & (1 << dir)) {
                    int row_type = y % 2;
                    int nx = x + dx[row_type][dir];
                    int ny = y + dy[dir];
                    
                    if (ny < 0 || ny >= height) {
                        int reverse_dir = (dir + 3) % 6;
                        grid_out[y][x].state |= (1 << reverse_dir);
                    } 
                    else if (nx < 0 || nx >= width) {
                        continue; 
                    } 
                    else {
                        if (grid_out[ny][nx].is_obstacle == false) {
                            grid_out[ny][nx].state |= (1 << dir);
                        } else {
                            int reverse_dir = (dir + 3) % 6;
                            grid_out[y][x].state |= (1 << reverse_dir);
                        }
                    }
                
                }
            }
            
            if (state & 0x40) {
                grid_out[y][x].state |= 0x40;
            }
        }
    }
}





void apply_inlet(Cell** grid, int width, int height, double rho_in) {
    (void)width;
    double prob = rho_in / 3.0; 
    int right_dirs[3] = {0, 1, 5};
    
    for (int y = 0; y < height; y++) {
        if (grid[y][0].is_obstacle) continue;

        for (int i = 0; i < 3; i++) {
            int dir = right_dirs[i];
            double r = (double)rand() / RAND_MAX;
            if (r < prob) {
                grid[y][0].state |= (1 << dir);
            }
        }
    }
}