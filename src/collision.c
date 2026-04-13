#include "../include/lga.h"
#include <stdlib.h>

void collision(Cell** grid, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x].is_obstacle == true) {
                continue;
            }
            uint8_t s = grid[y][x].state;
            if (rand() % 2 == 0) {
                grid[y][x].state = lut_a[s];
            } else {
                grid[y][x].state = lut_b[s];
            }
        }
    }
}