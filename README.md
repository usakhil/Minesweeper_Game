
# Minesweeper Game

## Project Overview

This project is a console-based Minesweeper game implemented in C++. It allows players to navigate a board, reveal tiles, and flag mines using keyboard inputs. The game features a real-time cursor update mechanism for an interactive gaming experience.

## Features

- **Navigation:** Move the cursor around the board using arrow keys.
- **Reveal Tiles:** Reveal tiles using the space bar.
- **Flag Mines:** Flag potential mines using the 'M' key.
- **Real-Time Updates:** The cursor position updates in real-time using a separate thread.
- **Main Menu:** A simple main menu to start the game.
- **High Scores:** View and record high scores.

## Getting Started

### Prerequisites

- A C++ compiler that supports C++11 or later.
- Windows operating system (due to the use of Windows-specific headers).

### Compilation

To compile the project, you can use any C++ compiler like `g++` or an IDE like Visual Studio.

Using `g++`, you can compile the project with the following command:
\`\`\`sh
g++ -o minesweeper main.cpp board.cpp tile.cpp cursor.cpp highscores.cpp -lwinmm
\`\`\`

### Running the Game

Once compiled, run the executable:
\`\`\`sh
./minesweeper
\`\`\`

### Controls

- **Arrow Keys:** Move the cursor (Up, Down, Left, Right).
- **Space Bar:** Reveal the tile at the cursor position.
- **M Key:** Flag the tile at the cursor position.
- **Escape Key:** Exit the current game.

## Project Structure

- \`main.cpp\`: Contains the main game loop and input handling.
- \`board.h\` and \`board.cpp\`: Defines the \`board\` class, which handles game logic, board updates, and drawing.
- \`tile.h\` and \`tile.cpp\`: Defines the \`Tile\` class, which represents each tile on the board.
- \`cursor.h\` and \`cursor.cpp\`: Defines the \`Cursor\` class, which manages the cursor's position and movement.
- \`highscores.h\` and \`highscores.cpp\`: Manages the high scores.

### Example \`main.cpp\`
\`\`\`cpp
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <random>
#include <fstream>
#include "windows.h"
#include "board.h"

using namespace std;

int main()
{
    board myBoard;
    while (myBoard.mainMenu()) {
        bool game_running = true;
        int time = 40; //auto_update_interval
        system("cls");
        thread thread_obj(&board::c_update, &myBoard, time, ref(game_running));  //thread for cursor update

        do {
            system("pause>nul"); //wait for key
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) {	// 0x8000 gives the real-time state of the key. will be false if key is not currently pressed.
                myBoard.move_down();					// prevents menu input from being queued up.
            }
            if (GetAsyncKeyState(VK_UP) & 0x8000) {
                myBoard.move_up();
            }
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
                myBoard.move_right();
            }
            if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
                myBoard.move_left();
            }
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                game_running = myBoard.revealTile();
            }
            if (GetAsyncKeyState(0x4D) & 0x8000) {	// m
                myBoard.flagTile();
            }
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                game_running = false;
            }
        } while (game_running);
        thread_obj.detach(); //stops cursor updates
        system("cls");
        myBoard.gameOver(); //draws fully revealed map
    }
}
\`\`\`

### Example \`board.h\`
\`\`\`cpp
#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <chrono>
#include <mutex>
#include "tile.h"
#include "cursor.h"
#include "highscores.h"
#include <windows.h>
#include <iostream>
#include <iomanip>

using namespace std;

const int EASY_DIFFICULTY = 10, MEDIUM_DIFFICULTY = 20, HARD_DIFFICULTY = 30;
const int SMALL_BOARD = 15, MEDIUM_BOARD = 20, LARGE_BOARD = 24;

class board
{
private:
    vector<Tile> tileVec;
    Highscores highscores;
    Cursor cursor;
    int boardWidth;
    int boardHeight;
    int revealedCount;
    int bombPercentage;
    int timeBonus;
    int numBombs;
    int flagCount;
    bool win;
    HANDLE console_color;
    mutex mut;

    void ShowConsoleCursor(bool showFlag);
    BOOL gotoxy(const WORD x, const WORD y);
    bool checkBounds(int offset, int currentPos);

public:
    board();
    void setSize(const int size);
    void setDifficulty(const int difficulty);
    void setBoard();
    void revealClearArea(int startX, int startY);
    void printBoard();
    void move_down();
    void move_up();
    void move_right();
    void move_left();
    void flagTile();
    bool revealTile();
    void gameOver();
    void printBanner();
    void displayMenu(const vector<string>& menu, const int& selection);
    int selectMenu(const vector<string>& menuList);
    void gameSettings();
    bool mainMenu();
    string recordScore(int score);
    void c_update(int time, bool& game_running);
};
#endif
\`\`\`

### Example \`cursor.h\`
\`\`\`cpp
#ifndef CURSOR_H
#define CURSOR_H

#include <string>

using namespace std;

class Cursor {
private:
    int shapeIndex;
    string cursorShapes;
protected:
    static int cursorPos;
public:
    Cursor() {
        cursorPos = 0;
        shapeIndex = 0;
        cursorShapes = "\|/-";
    }
    int getCursorPos() const { return cursorPos; }
    void setCursorPos(int newPos) { cursorPos = newPos; }
    char showCursor() {
        return cursorShapes.at((++shapeIndex < 4) ? shapeIndex : shapeIndex = 0);
    }
};
int Cursor::cursorPos = 0;
#endif
\`\`\`

### Example \`tile.h\`
\`\`\`cpp
#ifndef TILE_H
#define TILE_H
#include "cursor.h"

const char HIDDEN_CHAR = '-';
const char EMPTY_CHAR = ' ';
const char BOMB_CHAR = '*';
const char FLAG_CHAR = 'P';
const chrono::steady_clock::time_point START_TIME = chrono::steady_clock::now();

enum Status { hidden, flagged, revealed };

class Tile : public Cursor
{
protected:
    char tileValue;
    Status status;
public:
    Tile() {
        tileValue = EMPTY_CHAR;
        status = hidden;
    }
    void setTile(char c) { tileValue = c; }
    void setStatus(Status newStatus) { status = newStatus; }
    Status getStatus() const { return status; }
    char showTile(int loc) {
        char shownChar = 'X';
        if (loc == cursorPos) {
            shownChar = showCursor();
        }
        else {
            switch (status) {
            case hidden:
                shownChar = HIDDEN_CHAR;
                break;
            case flagged:
                shownChar = FLAG_CHAR;
                break;
            case revealed:
                shownChar = tileValue;
                break;
            }
        }
        return shownChar;
    }
    char getTile() { return tileValue; }
};
#endif
\`\`\`

## Contributing

We welcome contributions to enhance the Minesweeper game. To contribute, follow these steps:

1. Fork the repository.
2. Create a new branch (\`git checkout -b feature-branch\`).
3. Commit your changes (\`git commit -m 'Add new feature'\`).
4. Push to the branch (\`git push origin feature-branch\`).
5. Open a pull request.

## Acknowledgments

We thank the community and our users for their support and feedback, which help us continuously improve the Minesweeper game.

## Authors

This project was developed by Akhil Bhandari, Stanislav Dalgardno , and Kyle Caponio Driver.
