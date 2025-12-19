#include <ncurses.h>

int main() {
    initscr();
    keypad(stdscr, TRUE);
    noecho();  // Не отображать вводимые символы
    
    int ch;
    while((ch = getch()) != 'q') {
        clear();
        
        switch(ch) {
            case KEY_UP:
                printw("UP ARROW\n");
                break;
            case KEY_DOWN:
                printw("DOWN ARROW\n");
                break;
            case KEY_LEFT:
                printw("LEFT ARROW\n");
                break;
            case KEY_RIGHT:
                printw("RIGHT ARROW\n");
                break;
            case KEY_F(1):
                printw("F1 pressed\n");
                break;
            default:
                printw("Key code: %d\n", ch);
        }
        //refresh();
    }
    
    endwin();
    return 0;
}