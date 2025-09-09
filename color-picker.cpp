#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ncurses.h>
#include <iomanip>
#include <sstream>

// function to convert rgb to hex string

std::string rgbToHex(int r, int g, int b) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << r << std::setw(2) << g << std::setw(2) << b;
    return ss.str();
}

int main() {
    //init ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // check if terminal support color
    if (has_colors() == FALSE) {
        endwin();
        std::cout << "Your terminal does not support colors" << std::endl;
        return 1;
    }

    // Start color mode
    start_color();

    //create a color grid

    const int grid_size = 6;
    std::vector<std::vector<std::vector<int>>> colors(grid_size, std::vector<std::vector<int>>(grid_size, std::vector<int>(3)));

    //populate color grid with simple spectrum

    for (int r_idx = 0; r_idx < grid_size; ++r_idx) {
        for (int g_idx = 0; g_idx < grid_size; ++g_idx) {
            int r_val = (r_idx * 255) / (grid_size - 1);
            int g_val = (g_idx * 255) / (grid_size - 1);
            //simple blue component calculation
            int b_val = (r_val + g_val) / 2;

            //limit b_val to 255
            if (b_val > 255) b_val = 255;

            colors[r_idx][g_idx][0] = r_val;
            colors[r_idx][g_idx][1] = g_val;
            colors[r_idx][g_idx][2] = b_val;
        }
    }

    int cursor_y = 0;
    int cursor_x = 0;
    int ch;

    //main while loop
    while ((ch = getch()) != 'q') {
        //clear screen for redraw
        clear();

        // handle user input

        switch (ch) {
            case KEY_UP:
                cursor_y = (cursor_y > 0) ? cursor_y - 1 : cursor_y;
                break;
            case KEY_DOWN:
                cursor_y = (cursor_y < grid_size - 1) ? cursor_y + 1 : cursor_y;
                break;
            case KEY_LEFT:
                cursor_x = (cursor_x > 0) ? cursor_x - 1 : cursor_x;
                break;
            case KEY_RIGHT:
                cursor_x = (cursor_x < grid_size - 1) ? cursor_x : cursor_x;
                break;
        }

        //draw color grid

        for (int y = 0; y < grid_size; ++y) {
            for (int x = 0; x < grid_size; ++x) {
                int r = colors[y][x][0];
                int g = colors[y][x][1];
                int b = colors[y][x][2];

                //init new color pair for each square
                //map RGB to the 256 colors of ncurses

                int ncurses_color_idx = 16 + (r / 51) * 36 + (g / 51) * 6 + (b / 51);
                init_pair(y * grid_size + x + 1, ncurses_color_idx, ncurses_color_idx);

                if (y == cursor_y && x == cursor_x) {
                    attron(A_STANDOUT); //highlight selected color
                }

                attron(COLOR_PAIR(y * grid_size + x + 1));
                mvaddstr(y, x * 3, "   "); // draw a small square
                attroff(COLOR_PAIR(y * grid_size + x + 1));

                if (y == cursor_y && x == cursor_x) {
                    attroff(A_STANDOUT);
                }
            }
        }

        //display selected color information
        int r = colors[cursor_y][cursor_x][0];
        int g = colors[cursor_y][cursor_x][1];
        int b = colors[cursor_y][cursor_x][2];
        std::string hex = rgbToHex(r, g, b);

        mvprintw(grid_size + 2, 0, "Selected Color:");
        mvprintw(grid_size + 3, 0, "RGB: (%d, %d, %d)", r, g, b);
        mvprintw(grid_size + 4, 0, "Hex: #%s", hex.c_str());
        mvprintw(grid_size + 6, 0, "Use arrow keys to navigate. Press 'q' to quit.");

        //refresh function makes it so that changes become apparent
        refresh();
    }

    //end ncurses mode
    endwin();

    return 0;

}
