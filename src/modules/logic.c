#include "logic.h"
void next_generation(char map[][WIDTH], char mep_next[][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (is_alive(map, i, j)) {
                mep_next[i][j] = SYMBOL;
            } else {
                mep_next[i][j] = ' ';
            }
        }
    }
    array_copy(map, mep_next);
}

int is_alive(char map[][WIDTH], int y, int x) {
    int sum = sum_neighbours(map, y, x);
    int alive = 0;
    if (map[y][x] == SYMBOL) {
        alive = 1;
        sum--;
    }
    if (!(alive) && sum == 3) {
        return 1;
    } else if ((alive) && sum > 1 && sum < 4) {
        return 1;
    } else {
        return 0;
    }
}

int sum_neighbours(char map[][WIDTH], int y, int x) {
    int sub_y, sub_x, sum = 0;
    for (int i = -1; i < 2; i++) {
        sub_y = shift_y(y, i);
        for (int j = -1; j < 2; j++) {
            sub_x = shift_x(x, j);
            if (map[sub_y][sub_x] == SYMBOL) {
                sum += 1;
            }
        }
    }
    return sum;
}

int shift_y(int index, int shift_amount) {
    int new_index = (index + shift_amount) % HEIGHT;
    if (new_index < 0) {
        new_index += HEIGHT;
    }
    return new_index;
}

int shift_x(int index, int shift_amount) {
    int new_index = (index + shift_amount) % WIDTH;
    if (new_index < 0) {
        new_index += WIDTH;
    }
    return new_index;
}

void array_copy(char map[][WIDTH], char mep_next[][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            map[i][j] = mep_next[i][j];
        }
    }
}