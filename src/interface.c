#include <locale.h>
#include <ncurses.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

// Константы игры
#define WIDTH 80
#define HEIGHT 25
#define MAX_SPEED 20
#define MIN_SPEED 1

// Заменяем Unicode символы на ASCII для совместимости
#define ALIVE_CELL 'O'
#define DEAD_CELL ' '

// Структура для хранения состояния игры
typedef struct {
    char current[HEIGHT][WIDTH];
    char next[HEIGHT][WIDTH];
    int generation;
    int speed;
    int paused;
    int running;
} GameState;

// Структура для меню
typedef struct {
    char name[50];
    char filename[100];
} MenuItem;

// Глобальные переменные
GameState game;
MenuItem menu_items[] = {
    {"Glider", "states/glider.txt"},
    {"Pulsar", "states/pulsar.txt"},
    {"Spaceship", "states/spaceship.txt"},
    {"Gosper Gun", "states/gosper.txt"},
    {"Random", "states/random.txt"},
    {"Empty Field", "states/empty.txt"}
};
int menu_count = 6;

// Прототипы функций
void init_game();
void init_ncurses();
void draw_border();
void draw_game();
void update_game();
int count_neighbors(int x, int y);
void handle_input();
void show_menu();
void load_from_file(const char *filename);
void create_test_files();
void draw_info();

// Инициализация ncurses
void init_ncurses() {
    initscr();             // Инициализация ncurses
    cbreak();              // Режим посимвольного ввода
    noecho();              // Не отображать вводимые символы
    curs_set(0);           // Скрыть курсор
    keypad(stdscr, TRUE);  // Включить обработку специальных клавиш
    nodelay(stdscr, TRUE); // Неблокирующий ввод
    
    // Включаем цвета, если терминал их поддерживает
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Живые клетки
        init_pair(2, COLOR_WHITE, COLOR_BLACK);  // Информация
        init_pair(3, COLOR_CYAN, COLOR_BLACK);   // Меню
    }
}

// Инициализация игры
void init_game() {
    memset(game.current, DEAD_CELL, sizeof(game.current));
    memset(game.next, DEAD_CELL, sizeof(game.next));
    game.generation = 0;
    game.speed = 5;  // Средняя скорость
    game.paused = 1; // Начинаем на паузе
    game.running = 1;
}

// Отрисовка границы поля (упрощенная ASCII версия)
void draw_border() {
    // Верхняя и нижняя границы
    for (int x = 0; x < WIDTH + 2; x++) {
        mvaddch(0, x, '-');
        mvaddch(HEIGHT + 1, x, '-');
    }
    
    // Боковые границы
    for (int y = 0; y < HEIGHT + 2; y++) {
        mvaddch(y, 0, '|');
        mvaddch(y, WIDTH + 1, '|');
    }
    // Углы
    mvaddch(0, 0, '+');
    mvaddch(0, WIDTH + 1, '+');
    mvaddch(HEIGHT + 1, 0, '+');
    mvaddch(HEIGHT + 1, WIDTH + 1, '+');
}

// Отрисовка игрового поля
void draw_game() {
    clear();
    
    // Отрисовка границы
    draw_border();
    
    // Отрисовка клеток
    if (has_colors()) {
        attron(COLOR_PAIR(1));
    } else {
        attron(A_BOLD); // Если нет цветов, используем жирный шрифт
    }
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            mvaddch(y + 1, x + 1, game.current[y][x]);
        }
    }
    
    if (has_colors()) {
        attroff(COLOR_PAIR(1));
    } else {
        attroff(A_BOLD);
    }
    
    // Отрисовка информации
    draw_info();
    
    refresh();
}

// Подсчет соседей с учетом торического поля
int count_neighbors(int x, int y) {
    int count = 0;
    
    // Проверяем все 8 соседей
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue; // Пропускаем саму клетку
            
            // Вычисляем координаты с учетом торического поля
            int nx = (x + dx + WIDTH) % WIDTH;
            int ny = (y + dy + HEIGHT) % HEIGHT;
            
            if (game.current[ny][nx] == ALIVE_CELL) {
                count++;
            }
        }
    }
    
    return count;
}

// Обновление состояния игры
void update_game() {
    if (game.paused) return;
    
    // Вычисляем следующее поколение
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int neighbors = count_neighbors(x, y);
            
            if (game.current[y][x] == ALIVE_CELL) {
                // Правила для живой клетки
                if (neighbors < 2 || neighbors > 3) {
                    game.next[y][x] = DEAD_CELL; // Умирает
                } else {
                    game.next[y][x] = ALIVE_CELL; // Выживает
                }
            } else {
                // Правила для мертвой клетки
                if (neighbors == 3) {
                    game.next[y][x] = ALIVE_CELL; // Рождается
                } else {
                    game.next[y][x] = DEAD_CELL; // Остается мертвой
                }
            }
        }
    }
    
    // Копируем следующее состояние в текущее
    memcpy(game.current, game.next, sizeof(game.current));
    game.generation++;
}

// Обработка ввода (только английская раскладка)
void handle_input() {
    int ch = getch();
    
    switch(ch) {
        case 'a':
        case 'A':
            if (game.speed < MAX_SPEED) {
                game.speed++;
            }
            break;
            
        case 'z':
        case 'Z':
            if (game.speed > MIN_SPEED) {
                game.speed--;
            }
            break;
            
        case 32:
            game.paused = !game.paused;
            break;
            
        case 'm':
        case 'M':
            game.paused = 1;
            show_menu();
            break;
            
        case 27: // ESC
            game.running = 0;
            break;
            
        case 'r':
        case 'R':
            // Случайное заполнение
            srand(time(NULL));
            for (int y = 0; y < HEIGHT; y++) {
                for (int x = 0; x < WIDTH; x++) {
                    game.current[y][x] = (rand() % 5 == 0) ? ALIVE_CELL : DEAD_CELL;
                }
            }
            game.generation = 0;
            break;
            
        case 'c':
        case 'C':
            // Очистка поля
            memset(game.current, DEAD_CELL, sizeof(game.current));
            game.generation = 0;
            break;
    }
}

// Отрисовка информации
void draw_info() {
    if (has_colors()) {
        attron(COLOR_PAIR(2));
    }
    
    // Информация внизу экрана
    mvprintw(HEIGHT + 3, 2, "Generation: %d", game.generation);
    mvprintw(HEIGHT + 3, 25, "Speed: %d", game.speed);
    mvprintw(HEIGHT + 3, 45, "Status: %s", game.paused ? "PAUSED" : "RUNNING");
    
    // Управление
    mvprintw(HEIGHT + 5, 2, "Controls:");
    mvprintw(HEIGHT + 6, 4, "A - increase speed");
    mvprintw(HEIGHT + 7, 4, "Z - decrease speed");
    mvprintw(HEIGHT + 8, 4, "SPACE - pause/resume");
    mvprintw(HEIGHT + 9, 4, "R - random field");
    mvprintw(HEIGHT + 10, 4, "C - clear field");
    mvprintw(HEIGHT + 11, 4, "M - menu");
    mvprintw(HEIGHT + 12, 4, "ESC - exit");
    
    if (has_colors()) {
        attroff(COLOR_PAIR(2));
    }
}

// Загрузка состояния из файла
void load_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        file = fopen(filename, "r");
        if (!file) {
            mvprintw(HEIGHT + 15, 2, "Error: Cannot open file %s", filename);
            refresh();
            sleep(2);
            return;
        }
    }
    
    // Очищаем поле
    memset(game.current, DEAD_CELL, sizeof(game.current));
    
    char line[256];
    int x, y;
    int cells_loaded = 0;
    
    while (fgets(line, sizeof(line), file)) {
        // Пропускаем пустые строки и комментарии
        if (line[0] == '#' || line[0] == '\n') continue;
        
        // Читаем координаты
        if (sscanf(line, "%d %d", &x, &y) == 2) {
            // Проверяем границы
            if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
                game.current[y][x] = ALIVE_CELL;
                cells_loaded++;
            }
        }
    }
    
    fclose(file);
    game.generation = 0;
    
    // Показываем сообщение о загрузке
    clear();
    mvprintw(10, 30, "Loaded %d cells from %s", cells_loaded, filename);
    mvprintw(12, 30, "Press any key to continue...");
    refresh();
    getch();
}

// Показать меню выбора начального состояния
void show_menu() {
    int selected = 0;
    int ch;
    
    // Сохраняем текущее состояние экрана
    char saved_screen[HEIGHT][WIDTH];
    memcpy(saved_screen, game.current, sizeof(saved_screen));
    
    while (1) {
        clear();
        
        // Заголовок меню
        if (has_colors()) {
            attron(COLOR_PAIR(3) | A_BOLD);
        } else {
            attron(A_BOLD);
        }
        mvprintw(5, 30, "SELECT INITIAL STATE");
        if (has_colors()) {
            attroff(COLOR_PAIR(3) | A_BOLD);
        } else {
            attroff(A_BOLD);
        }
        
        // Пункты меню
        for (int i = 0; i < menu_count; i++) {
            if (i == selected) {
                attron(A_REVERSE);
            }
            mvprintw(10 + i * 2, 35, "%d. %s", i + 1, menu_items[i].name);
            if (i == selected) {
                attroff(A_REVERSE);
            }
        }
        
        // Подсказка
        mvprintw(25, 30, "UP/DOWN - select | ENTER - load | ESC - back");
        
        refresh();
        
        ch = getch();
        
        switch(ch) {
            case KEY_UP:
                selected = (selected - 1 + menu_count) % menu_count;
                break;
                
            case KEY_DOWN:
                selected = (selected + 1) % menu_count;
                break;
                
            case '\n': // ENTER
                // Загружаем выбранный файл
                load_from_file(menu_items[selected].filename);
                return;
                
            case '1' ... '6': // Быстрый выбор цифрами
                selected = ch - '1';
                if (selected < menu_count) {
                    load_from_file(menu_items[selected].filename);
                    return;
                }
                break;
                
            case 27: // ESC
                // Восстанавливаем предыдущее состояние
                memcpy(game.current, saved_screen, sizeof(saved_screen));
                return;
        }
    }
}

// Главный игровой цикл
void game_loop() {
    // Показываем меню при старте
    show_menu();
    
    // Основной игровой цикл
    while (game.running) {
        // Обработка ввода
        handle_input();
        
        // Обновление состояния
        if (!game.paused) {
            update_game();
        }
        
        // Отрисовка
        draw_game();
        
        // Задержка (управление скоростью)
        usleep(1000000 / game.speed);
    }
}

// Главная функция
int main() {
    // Установка локали
    setlocale(LC_ALL, "");
    
    // Инициализация ncurses
    init_ncurses();
    
    // Проверяем размер терминала
    int term_height, term_width;
    getmaxyx(stdscr, term_height, term_width);
    
    if (term_height < HEIGHT + 15 || term_width < WIDTH + 10) {
        endwin();
        printf("Terminal too small! Minimum size: %dx%d\n", WIDTH + 10, HEIGHT + 15);
        printf("Your terminal: %dx%d\n", term_width, term_height);
        return 1;
    }
    
    // Инициализация игры
    init_game();
    
    // Запуск игрового цикла
    game_loop();
    
    // Завершение работы с ncurses
    endwin();
    
    printf("Game ended. Goodbye!\n");
    
    return 0;
}