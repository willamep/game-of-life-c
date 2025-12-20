#ifndef UI_H
#define UI_H

#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>

#include "logic.h"

#define INPUT_DELAY 10000
#define MAX_DELAY 1000000  // Max = 1 sec
#define STEP_DELAY 100000

void init_ncurses(void);
void handle_input(char ch, int* delay, int* pause);
int input(char map[][WIDTH]);
void output_map(char map[][WIDTH]);
void output_info(int delay, int pause, long int gen);
void output_delay_bar(int delay);

#endif
