#ifndef LGA_H
#define LGA_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t state;    
    bool is_obstacle; 
} Cell;


extern int dy[6];
extern int dx[2][6];
extern uint8_t lut_a[128];
extern uint8_t lut_b[128];

void init_lut(void);
Cell** allocate_grid(int width, int height);
void init_obstacles(Cell** grid, int width, int height, int diameter);
void init_particles(Cell** grid, int width, int height, double rho);

void collision(Cell** grid, int width, int height);

void streaming(Cell** grid_in, Cell** grid_out, int width, int height);
void apply_inlet(Cell** grid, int width, int height, double rho_in);

void save_ppm(Cell** grid, int width, int height, int frame);
void calculate_and_save_density(Cell** grid, int width, int height);
void save_bin(Cell** grid, int width, int height);
void save_txt(double avg_rho);

#endif