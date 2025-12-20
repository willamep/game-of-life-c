#include "modules/logic.h"  // логика + HEIGHT/WIDTH/SYMBOL
#include "modules/ui.h"     // интерфейс + input() + тайминговые константы

#define INPUT_DELAY 10000
#define MAX_DELAY 1000000  // Max = 1 sec
#define STEP_DELAY 100000

int main(void) {
    // Создаём массив для карты и для просчёта следующего положения + переменную для символа с клавиатуры
    char map[HEIGHT][WIDTH], mep_next[HEIGHT][WIDTH], ch;
    int delay = 100000, pause = 0, timer_delay = 0;
    long int gen = 0; // Счётчик поколений
    // Считываем карту из перенаправленного потока ввода.
    if (input(map)) {
        printf("n/a");
        return 0;
    }
    init_ncurses(); // Инциализируем режим ncurses
    // Возвращаем поток ввода назад на клавиатуру, чтобы 
    if (!freopen("/dev/tty", "r", stdin)) {
        printf("n/a");
        return 0;
    }

    while ((ch = getch()) != 'q') {
        handle_input(ch, &delay, &pause);
        if (!pause && (timer_delay += INPUT_DELAY) >= delay) {
            timer_delay = 0;
            next_generation(map, mep_next);
            output_map(map);
            gen++;
        }
        output_info(delay, pause, gen);
        usleep(INPUT_DELAY);
        refresh();
    }
    return 0;
}