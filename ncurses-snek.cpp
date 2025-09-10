#include <ncurses.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <unistd.h>

//screen dimensions
const int MAX_Y = 25;
const int MAX_X =  80;

//game states
enum Direction { UP, DOWN, LEFT, RIGHT };

//snake & food
struct Point {
    int y, x;
};

//global vars
std::vector<Point> snake;
Point food;
Direction direction;
int score = 0;
bool gameOver = false;
bool isPaused = false;
int usleepDelay = 100000;

//init ncurses

void setup() {
    initscr(); //start ncurses mode
    cbreak(); //line buffering disabled, pass chars to program directly
    noecho(); //dont echo input chars
    keypad(stdscr, TRUE); //Enable special keys
    curs_set(0); // hide cursor
    timeout(100); //wait 100ms for input
    srand(time(0)); //seed random number generator
}

//draw game board and elements

void draw() {
    clear();
    box(stdscr, 0, 0);

    // Draw the snake's head
    if (!snake.empty()) {
        mvaddch(snake.front().y, snake.front().x, 'O'); // 'O' for the head
    }

    // Draw the rest of the snake's body
    for (size_t i = 1; i < snake.size(); ++i) {
        mvaddch(snake[i].y, snake[i].x, 'o'); // 'o' for the body
    }

    //draw food
    mvaddch(food.y, food.x, '@');

    //display score
    mvprintw(0, 3, "Score %d", score);

    if (isPaused) {
        mvprintw(MAX_Y / 2, MAX_X / 2 - 6, "GAME PAUSED");
    }

    refresh();
}


//generate food @ random position
void generateFood() {
    bool onSnake;
    do {
        onSnake = false;
        food.y = rand() % (MAX_Y - 2) + 1;
        food.x = rand() % (MAX_X - 2) + 1;

        // check if food is on the snake
        for (const auto& p : snake) {
            if (p.y == food.y && p.x == food.x) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
}

//update game state
void update() {
    if (gameOver) return;

    //get current head pose
    Point head = snake.front();

    //move snake
    switch (direction) {
        case UP:    head.y--; break;
        case DOWN:  head.y++; break;
        case LEFT:  head.x--; break;
        case RIGHT: head.x++; break;
    }

    //check for collision with walls
    if (head.y <= 0 || head.y >= MAX_Y || head.x <= 0 || head.x >= MAX_X - 1) {
        gameOver = true;
        return;
    }

    //check self collision
    for (const auto& p : snake) {
        if (p.y == head.y && p.x == head.x) {
            gameOver = true;
            return;
        }
    }

    //add new head to front of snake
    snake.insert(snake.begin(), head);

    //check if snake eats food
    if (head.y == food.y && head.x == food.x) {
        score += 10;
        generateFood();
        if (score % 50 == 0) {
            usleepDelay -= 10000;
        }
    } else {
        snake.pop_back();
    }
}

//handle user input
void input() {
    int ch = getch();
    switch (ch) {
        case KEY_UP:    if (direction != DOWN) direction = UP; break;
        case KEY_DOWN:  if (direction != UP) direction = DOWN; break;
        case KEY_LEFT:  if (direction != RIGHT) direction = LEFT; break;
        case KEY_RIGHT: if (direction != LEFT) direction = RIGHT; break;
        case 'q':       gameOver = true; break;
        case 'p':       isPaused = !isPaused; break;
    }
}

void resetGame() {

    snake.clear();
    //initial snake position and direction
    snake.push_back({MAX_Y / 2, MAX_X / 2});
    direction = RIGHT;
    score = 0;
    gameOver = false;
    isPaused = false;
    usleepDelay = 100000;
    //init food
    generateFood();
}

//main loop
int main() {
    setup();
    bool playing = true;

    while (playing) {
        //function to reset variables and init new game
        resetGame();


        //main loop
        while (!gameOver) {
            draw();
            input();

            if (!isPaused) {
                update();
            }

            usleep(usleepDelay);
        }

        bool choiceMade = false;
        while (!choiceMade) {
            //game over screen
            clear();
            mvprintw(MAX_Y / 2, MAX_X / 2 - 10, "Game Over ! Final Score : %d", score);
            mvprintw(MAX_Y / 2, MAX_X / 2 - 15, "Press 'r' to restart or 'q' to quit");
            refresh();

            int ch = getch();
            if (ch == 'q') {
                playing = false;
                choiceMade = true;
            } else if (ch == 'r') {
                gameOver = false;
                choiceMade = true;
            }
        }
    }
    //end ncurses mode
    endwin();
    return 0;
}











