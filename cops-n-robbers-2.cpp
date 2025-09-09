#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <map>
#include <tuple>
#include <algorithm> // This header is needed for std::reverse

// --- Cross-platform includes for console input and sleep ---
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <stdio.h>
#endif

// --- Constants for game elements ---
const char WALL = '#';
const char ROBBER = 'R';
const char COP = 'C';
const char COLLECTIBLE = 'o';
const char EMPTY = ' ';

// --- ANSI color codes for terminal coloring ---
#ifdef _WIN32
// Note: Windows Command Prompt may not support ANSI codes by default.
// This is a basic setup; more robust solutions exist.
const char* RESET_COLOR = "";
const char* RED_COLOR = "";
const char* BLUE_COLOR = "";
const char* YELLOW_COLOR = "";
const char* GRAY_COLOR = "";
const char* PURPLE_BG = "";
#else
const char* RESET_COLOR = "\033[0m";
const char* RED_COLOR = "\033[31m";
const char* BLUE_COLOR = "\033[34m";
const char* YELLOW_COLOR = "\033[33m";
const char* GRAY_COLOR = "\033[37m";
const char* PURPLE_BG = "\033[45m"; // ANSI code for a deep purple background
#endif

// --- Game state variables ---
std::vector<std::string> gameMap = {
    "#####################################",
    "#o o o o o o o o o o o o o o o o o o#",
    "#o ########### #o# # #############o#",
    "#  #         # #o# # #           # #",
    "#o # ####### # # #o# # ########### #",
    "#  # #     # # # # #o# #         #o#",
    "#o # # ### # # # ### #o# ####### # #",
    "#  # # # # # # # #   # #o#     # # #",
    "#o # # # #o# # #o### # # # ### #o#o#",
    "#  # # #o# # # # # # # # # # # # #o#",
    "#  ### # # # #o# # # # # #o# ### #o#",
    "#o o o # #o# # # # # # #o# # #   # #",
    "# # #o# #o# # #o# #o# # # # # #o#o#",
    "# # # #o#o# # # # # # # #o# # # # #",
    "# #o# # # #o# # # # # # # #o# # # #",
    "# # # # # # # # # # # # # # # # #o#",
    "# # # # # #o# # # # # # # # # # # #",
    "#o o o o o o o o o o o o o o o o o o#",
    "#####################################"
};

int robberX = 1;
int robberY = 1;
int copX = 33;
int copY = 17;
int score = 0;
int collectiblesLeft = 32; // Updated count for the new map
bool gameOver = false;
char currentRobberDirection = ' '; // New variable to store the robber's continuous movement direction

// Struct to represent a node in the pathfinding grid
struct Node {
    int x, y;
    int gScore, hScore, fScore;

    // A* requires a way to compare nodes in the priority queue
    bool operator>(const Node& other) const {
        return fScore > other.fScore;
    }
};

// Function to move the cursor to a specific position (x, y)
void gotoxy(int x, int y) {
#ifdef _WIN32
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    printf("\033[%d;%dH", y, x);
#endif
}

// Function to print the game map to the console
void drawMap() {
    // Move cursor to top-left to redraw without clearing
    gotoxy(1, 1);

    std::cout << "Score: " << score << " | Collectibles Left: " << collectiblesLeft << std::endl;
    for (const auto& row : gameMap) {
        for (char c : row) {
            std::cout << PURPLE_BG;
            if (c == ROBBER) {
                std::cout << RED_COLOR << c;
            } else if (c == COP) {
                std::cout << BLUE_COLOR << c;
            } else if (c == COLLECTIBLE) {
                std::cout << YELLOW_COLOR << c;
            } else if (c == WALL) {
                std::cout << GRAY_COLOR << c;
            } else {
                std::cout << c;
            }
            std::cout << RESET_COLOR;
        }
        std::cout << std::endl;
    }
}

// Function to handle player movement
void moveRobber(char move) {
    int newX = robberX;
    int newY = robberY;

    if (move == 'w' || move == 'W') newY--;
    else if (move == 's' || move == 'S') newY++;
    else if (move == 'a' || move == 'A') newX--;
    else if (move == 'd' || move == 'D') newX++;

    // Check for collisions
    if (newY >= 0 && newY < gameMap.size() && newX >= 0 && newX < gameMap[0].size() && gameMap[newY][newX] != WALL) {
        // Clear old position
        gameMap[robberY][robberX] = EMPTY;
        robberX = newX;
        robberY = newY;

        // Check for collectibles
        if (gameMap[robberY][robberX] == COLLECTIBLE) {
            score++;
            collectiblesLeft--;
            // Remove collectible by setting the position to empty
            gameMap[robberY][robberX] = EMPTY;
        }

        // Update the robber's position on the map
        gameMap[robberY][robberX] = ROBBER;
    }
}

// A* pathfinding algorithm
std::vector<std::pair<int, int>> findPath(int startX, int startY, int endX, int endY) {
    // A* algorithm implementation
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::map<std::pair<int, int>, std::pair<int, int>> cameFrom;
    std::map<std::pair<int, int>, int> gScore;

    // Heuristic function (Manhattan distance)
    auto heuristic = [](int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    };

    openSet.push({startX, startY, 0, heuristic(startX, startY, endX, endY), heuristic(startX, startY, endX, endY)});
    gScore[{startX, startY}] = 0;

    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();

        if (current.x == endX && current.y == endY) {
            // Reconstruct path
            std::vector<std::pair<int, int>> path;
            std::pair<int, int> currentPos = {endX, endY};
            while (cameFrom.count(currentPos)) {
                path.push_back(currentPos);
                currentPos = cameFrom[currentPos];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        // Explore neighbors (up, down, left, right)
        int dx[] = {0, 0, 1, -1};
        int dy[] = {1, -1, 0, 0};

        for (int i = 0; i < 4; ++i) {
            int neighborX = current.x + dx[i];
            int neighborY = current.y + dy[i];

            if (neighborX >= 0 && neighborX < gameMap[0].size() && neighborY >= 0 && neighborY < gameMap.size() && gameMap[neighborY][neighborX] != WALL) {
                int tentativeGScore = gScore[{current.x, current.y}] + 1;
                
                if (tentativeGScore < gScore[{neighborX, neighborY}] || !gScore.count({neighborX, neighborY})) {
                    cameFrom[{neighborX, neighborY}] = {current.x, current.y};
                    gScore[{neighborX, neighborY}] = tentativeGScore;
                    int hScore = heuristic(neighborX, neighborY, endX, endY);
                    int fScore = tentativeGScore + hScore;
                    openSet.push({neighborX, neighborY, tentativeGScore, hScore, fScore});
                }
            }
        }
    }

    // No path found
    return {};
}

// Simple AI for the cop: chase the robber using A*
void moveCop() {
    // Calculate the path to the robber
    auto path = findPath(copX, copY, robberX, robberY);
    
    // Move to the next tile on the path
    if (!path.empty()) {
        // Clear old cop position
        gameMap[copY][copX] = EMPTY;

        // Move the cop to the next tile in the path
        copX = path[0].first;
        copY = path[0].second;

        // Update cop's position on the map
        gameMap[copY][copX] = COP;

        // Check for collision after the move
        if (copX == robberX && copY == robberY) {
            gameOver = true;
        }
    }
}

// Main game loop
void gameLoop() {
    char input;
    static int frameCounter = 0;
    
    // Initial setup on the map
    gameMap[robberY][robberX] = ROBBER;
    gameMap[copY][copX] = COP;

    // --- Cross-platform function for non-blocking input ---
#ifndef _WIN32
    // Setup for non-blocking input on Unix-like systems
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif

    while (!gameOver && collectiblesLeft > 0) {
        drawMap();
        
        // Check for user input and update the continuous direction
#ifdef _WIN32
        if (_kbhit()) {
            input = _getch();
            currentRobberDirection = input;
        }
#else
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        struct timeval timeout = {0, 0};
        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &timeout) > 0) {
            read(STDIN_FILENO, &input, 1);
            currentRobberDirection = input;
        }
#endif
        
        // Move the robber based on the stored direction
        moveRobber(currentRobberDirection);

        // Move the cop 2 times for every 3 frames
        frameCounter++;
        if (frameCounter % 3 != 0) {
            moveCop();
        }

        // Check if the robber has been caught
        if (robberX == copX && robberY == copY) {
            gameOver = true;
        }

        // Wait for a moment to slow down the game
#ifdef _WIN32
        Sleep(200);
#else
        usleep(200000); // 200 milliseconds
#endif
    }

#ifndef _WIN32
    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    // End game screen
    drawMap();
    if (collectiblesLeft == 0) {
        std::cout << "\nCongratulations! You collected all the items and won!" << std::endl;
    } else {
        std::cout << "\nGame Over! The cops caught the robber!" << std::endl;
    }
    std::cout << "Final Score: " << score << std::endl;
}

int main() {
    std::cout << "Welcome to Cops and Robbers!" << std::endl;
    std::cout << "Use W, A, S, D to move. Collect all the 'o's without getting caught!" << std::endl;
    std::cout << "Press any key to start..." << std::endl;
#ifdef _WIN32
    _getch();
#else
    char c;
    read(STDIN_FILENO, &c, 1);
#endif
    gameLoop();
    return 0;
}

