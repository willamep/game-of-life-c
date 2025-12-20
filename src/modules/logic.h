#ifndef LOGIC_H
#define LOGIC_H

#define HEIGHT 25
#define WIDTH 80
#define SYMBOL '@'

void next_generation(char map[][WIDTH], char mep_next[][WIDTH]);
int is_alive(char map[][WIDTH], int y, int x);
int sum_neighbours(char map[][WIDTH], int y, int x);
int shift_y(int index, int shift_amount);
int shift_x(int index, int shift_amount);
void array_copy(char map[][WIDTH], char mep_next[][WIDTH]);

#endif
