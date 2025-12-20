#include "ui.h"

void init_ncurses() {
    // Базовая инициализация
    initscr();              // Инициализация ncurses
    cbreak();               // Отключаем буферизацию ввода
    noecho();               // Не показываем нажатые клавиши
    keypad(stdscr, TRUE);   // Включаем специальные клавиши
    nodelay(stdscr, TRUE);  // Неблокирующий getch()
    curs_set(0);            // Скрываем курсор
    // Задаём цвета
    start_color();                  // Включаем цвета в ncurses
    use_default_colors();           // Устанавливаем фон, как в терминале (прозрачный)
    init_pair(1, COLOR_GREEN, -1);  // Создаём цветовую пару для символов клеток.
}

// Обработка нажатий
void handle_input(char ch, int* delay, int* pause) {
    if ((ch == 'a' || ch == 'A') && *delay < MAX_DELAY) {
        *delay += STEP_DELAY;
    } else if ((ch == 'z' || ch == 'Z') && *delay > INPUT_DELAY) {
        *delay -= STEP_DELAY;
    } else if (ch == ' ') {
        *pause = !*pause;
    }
}

// Вводим карту в матрицу
int input(char map[][WIDTH]) {
    char ch = ' ';
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            ch = getchar();  // Получаем символ
            // Если символ не равен переносу, то обрабатываем.
            if (ch != '\n') {
                if (ch == '1' || ch == 'O' || ch == SYMBOL) {
                    map[i][j] = SYMBOL;
                } else {
                    map[i][j] = ' ';
                }
            } else {
                j--;
            }  // При символе переноса строки уменьшаем j, чтобы не сосчитать меньшее количество символов.
        }
    }
    return 0;
}

// Отрисовка карты
void output_map(char map[][WIDTH]) {
    erase();  // Предотвращает полную перерисовку
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            attron(COLOR_PAIR(1));     // Применяем цветовой пресет для следующего вывода
            mvaddch(i, j, map[i][j]);  // Выводим символ в точных координатах
            attroff(COLOR_PAIR(1));    // Выключаем пресет
        }
    }
}

void output_info(int delay, int pause, long int gen) {
    // Отрисовываем строку инфо (под полем)
    mvprintw(HEIGHT + 1, 0, "A: Speed down | Z: Speed up | Space: Pause | Q: Exit");
    output_delay_bar(delay);
    mvprintw(HEIGHT + 2, 30, "Pause: %s", pause ? "no" : "yes");
    mvprintw(HEIGHT + 2, 45, "Gen: %ld", gen);
}

void output_delay_bar(int delay) {
    mvprintw(HEIGHT + 2, 0, "Speed: ");
    int level = 10 - delay / 100000;
    printw("{");
    for (int i = 0; i < 10; i++) {
        if (i < level) {
            printw("#");
        } else {
            printw("-");
        }
    }
    printw("} %d%%", level * 10);
}