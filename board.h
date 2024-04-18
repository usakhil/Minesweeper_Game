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

	void ShowConsoleCursor(bool showFlag) {
		//-------do not change-------
		//hides the cursor, use only once at program start
		HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO     cursorInfo;
		GetConsoleCursorInfo(out, &cursorInfo);
		cursorInfo.bVisible = showFlag; // set the cursor visibility
		SetConsoleCursorInfo(out, &cursorInfo);
	}
	BOOL gotoxy(const WORD x, const WORD y) {
		//-------do not change-------
		COORD xy;
		xy.X = x;
		xy.Y = y;
		return SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
	}
	bool checkBounds(int offset, int currentPos) {
		bool boundsValid = false;
		boundsValid = (currentPos + offset < tileVec.size()) && (currentPos + offset > -1);

		if (offset == 1 || offset == -1) {									// if going left-right check to avoid wrapping
			boundsValid = (currentPos % boardWidth + offset < boardWidth
				&& currentPos % boardWidth + offset > -1)
				? boundsValid : false;
		}
		else if (offset == boardWidth + 1 || offset == -boardWidth + 1) {	// Right side diagonals
			boundsValid = (currentPos % boardWidth != boardWidth - 1) ? boundsValid : false;
		}
		else if (offset == -boardWidth - 1 || offset == boardWidth - 1) {	// Left side diagonals
			boundsValid = (currentPos % boardWidth != 0) ? boundsValid : false;
		}
		return boundsValid;
	}
public:
	board() { 
		boardWidth = 1;
		boardHeight = 1;
		bombPercentage = 1;
		numBombs = 0;
		flagCount = 0;
		timeBonus = 0;
		revealedCount = 0;
		win = false;
		console_color = GetStdHandle(STD_OUTPUT_HANDLE);
		ShowConsoleCursor(false);
	}
	void setSize(const int size) {
		boardWidth = size;
		boardHeight = size;
		tileVec.clear();
		tileVec.resize(boardWidth * boardHeight);
	}
	void setDifficulty(const int difficulty) {
		bombPercentage = difficulty;
		numBombs = 0;
		flagCount = 0;
		revealedCount = 0;
		win = false;
	}
	void setBoard() {
		random_device rd;
		default_random_engine generator(rd());
		uniform_int_distribution<int> distribution(0, boardWidth * boardHeight - 1);
		uniform_int_distribution<int> range(5, bombPercentage);
		timeBonus = 8999;
		numBombs = boardWidth * boardHeight * range(generator) / 100;
		bombPercentage = numBombs;
		vector<int> adjTiles{ -boardWidth - 1, -boardWidth, -boardWidth + 1, -1, +1, boardWidth - 1, boardWidth, boardWidth + 1 };
		for (int i = 0; i < numBombs; i++)
		{
			int num = distribution(generator);
			if (num == cursor.getCursorPos() || tileVec.at(num).getTile() == BOMB_CHAR)
			{
				i--;
			}
			else
			{
				tileVec.at(num).setTile(BOMB_CHAR);
				for (int index : adjTiles) {																// increments all adjacent tiles characters
					if (checkBounds(index, num) && tileVec.at(index + num).getTile() != BOMB_CHAR) {		// avoid wrapping and incrementing bombs
						(tileVec.at(index + num).getTile() == EMPTY_CHAR)									// EMPTY_CHAR = ' '
							? tileVec.at(index + num).setTile(tileVec.at(index + num).getTile() + 17)		// ' ' + 17 = 1
							: tileVec.at(index + num).setTile(tileVec.at(index + num).getTile() + 1);
					}
				}
			}
		}
	}
	void revealClearArea(int startX, int startY) { // Changed the function to non-recursive
		vector<int> xCoords;
		vector<int> yCoords;

		xCoords.push_back(startX);
		yCoords.push_back(startY);

		int index = 0;
		while (index < xCoords.size()) {
			int currentX = xCoords.at(index);
			int currentY = yCoords.at(index);
			index++;

			int pos = currentY * boardWidth + currentX;

			// Check bounds and whether the tile has already been revealed
			if (currentX >= 0 && currentX < boardWidth && currentY >= 0 && currentY < boardHeight && tileVec.at(pos).getStatus() == hidden) {
				tileVec.at(pos).setStatus(revealed);
				++revealedCount;

				if (tileVec.at(pos).getTile() == EMPTY_CHAR) {  // Add adjacent tiles if within bounds
					for (int dx = -1; dx <= 1; ++dx) {
						for (int dy = -1; dy <= 1; ++dy) {
							if (dx == 0 && dy == 0) {
								// Made it blank to skip the tile itself
							}
							else {
								int newX = currentX + dx;
								int newY = currentY + dy;

								if (newX >= 0 && newX < boardWidth && newY >= 0 && newY < boardHeight) {    // Add adjacent tile to the vectors if within bounds
									xCoords.push_back(newX);
									yCoords.push_back(newY);
								}
							}
						}
					}
				}
			}
		}
	}
	void printBoard() {
		cout << setw(boardWidth + 5) << setfill(' ') << right << "MINESWEEPER" << endl;
		for (unsigned int i = 0; i < tileVec.size(); i++)
		{
			if (tileVec.at(i).getStatus() == flagged) {
				SetConsoleTextAttribute(console_color, 5);
			}
			if (tileVec.at(i).getTile() == BOMB_CHAR && tileVec.at(i).getStatus() == revealed) {
				SetConsoleTextAttribute(console_color, 4);
			}
			i% boardWidth || i == 0 ? cout << " " << tileVec.at(i).showTile(i) : cout << endl << " " << tileVec.at(i).showTile(i);
			SetConsoleTextAttribute(console_color, 15);
		}
		cout << endl << endl;
		cout << "Number of bombs: " << numBombs << endl;
		cout << "Number of flagged tiles: " << flagCount << endl;
		cout << "Score Bonus : " << setw(3) << setfill('0') << right << timeBonus / 9 << endl << setfill(' ');
	}

	void move_down() {
		mut.lock();
		if (checkBounds(boardWidth, cursor.getCursorPos())) {
			cursor.setCursorPos(cursor.getCursorPos() + boardWidth);
		}
		mut.unlock();
	}
	void move_up() {
		mut.lock();
		if (checkBounds(-boardWidth, cursor.getCursorPos())) {
			cursor.setCursorPos(cursor.getCursorPos() - boardWidth);
		}
		mut.unlock();
	}
	void move_right() {
		mut.lock();
		if (checkBounds(1, cursor.getCursorPos())) {
			cursor.setCursorPos(cursor.getCursorPos() + 1);
		}
		mut.unlock();
	}
	void move_left() {
		mut.lock();
		if (checkBounds(-1, cursor.getCursorPos())) {
			cursor.setCursorPos(cursor.getCursorPos() - 1);
		}
		mut.unlock();
	}
	void flagTile() {
		mut.lock();
		if (tileVec.at(cursor.getCursorPos()).getStatus() == hidden) {
			tileVec.at(cursor.getCursorPos()).setStatus(flagged);
			++flagCount;
		}
		else if (tileVec.at(cursor.getCursorPos()).getStatus() == flagged) {
			tileVec.at(cursor.getCursorPos()).setStatus(hidden);
			--flagCount;
		}
		mut.unlock();
	}
	bool revealTile() {
		bool stillPlaying = true;
		if (tileVec.at(cursor.getCursorPos()).getStatus() != flagged) { // Player must remove flag to reveal tile
			mut.lock();
			if (numBombs == 0) {
				setBoard();
			}
			if (tileVec.at(cursor.getCursorPos()).getTile() == BOMB_CHAR) {
				stillPlaying = false;
			}
			else {
				int x = cursor.getCursorPos() % boardWidth;
				int y = cursor.getCursorPos() / boardWidth;
				revealClearArea(x, y);
			}
			if (boardWidth * boardHeight - numBombs == revealedCount) {
				stillPlaying = false;
				win = true;
			}
			mut.unlock();
		}
		return stillPlaying;
	}
	void gameOver() {
		string winorlose;
		highscore player;
		player.score = revealedCount / (boardWidth / 4) + (((timeBonus / 9) + (numBombs * 5)) * win);
		for (Tile& tile : tileVec) {
			tile.setStatus(revealed);
		}
		printBoard();
		winorlose = (win) ? "win!" : "lose...";
		cout << "GAME OVER" << endl
			<< "You " << winorlose << endl 
			<< endl
			<< "Score: " << player.score << endl 
			<< endl
			<< "Press anything to record your score" << endl;
		system("pause>nul"); //wait for key
		player.name = recordScore(player.score);
		highscores.addScore(player);
		highscores.sortHighscores();
		highscores.updateHighscores();
	}

	void printBanner() {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
		WORD saved_attributes = consoleInfo.wAttributes;

		int consoleWidth = consoleInfo.dwSize.X;

		SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		cout << setw(consoleWidth) << left << "  __  __ _               _____" << endl
			<< setw(consoleWidth) << left << " |  \\/  (_)             / ____|                                 " << endl
			<< setw(consoleWidth) << left << " | \\  / |_ _ __   ___  | (_____      _____  ___ _ __   ___ _ __ " << endl
			<< setw(consoleWidth) << left << " | |\\/| | | '_ \\ / _ \\  \\___ \\ \\ /\\ / / _ \\/ _ \\ '_ \\ / _ \\ '__|" << endl
			<< setw(consoleWidth) << left << " | |  | | | | | |  __/  ____) \\ V  V /  __/  __/ |_) |  __/ |   " << endl
			<< setw(consoleWidth) << left << " |_|  |_|_|_| |_|\___|  |_____/ \\_/\\_/ \\___|\\___| .__/ \\___|_|   " << endl
			<< setw(consoleWidth) << left << "                                               | |              " << endl
			<< setw(consoleWidth) << left << "                                               |_|              " << endl;
		cout << endl;
		SetConsoleTextAttribute(hConsole, saved_attributes);
	}
	void displayMenu(const vector<string>& menu, const int& selection) { // displays menu options, highlights current selection
		HANDLE console_color = GetStdHandle(STD_OUTPUT_HANDLE);
		for (int i = 0; i < menu.size(); ++i) {
			if (selection == i) {
				SetConsoleTextAttribute(console_color, 23);
				cout << setw(10 + menu.at(i).size()) << setfill(' ') << right << menu.at(i) << endl;
				SetConsoleTextAttribute(console_color, 15);
			}
			else {
				cout << setw(10 + menu.at(i).size()) << setfill(' ') << right << menu.at(i) << endl;
			}
		}
		cout << endl << " Press enter to select" << endl << endl;
	}
	int selectMenu(const vector<string>& menuList) { // keeps track of user input for menu
		int menuSelection = 1;
		bool running = true;
		system("cls"); 
		while (running) {
			gotoxy(0, 0); // prevent flickering while navigating menu
			printBanner();
			displayMenu(menuList, menuSelection);
			system("pause>nul"); //wait for key
			if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
				menuSelection == menuList.size() - 1 ? menuSelection : menuSelection++;
			}
			if (GetAsyncKeyState(VK_UP) & 0x8000) {
				menuSelection == 1 ? menuSelection : menuSelection--;
			}
			if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
				running = false;
			}
		}
		return menuSelection;
	}
	void gameSettings() {
		vector <string> sizeVec{ "SELECT BOARD SIZE", "Small", "Medium", "Large" };
		vector <string> difficultyVec{ "SELECT DIFFICULTY", "Easy", "Medium", "Hard" };
		switch (selectMenu(sizeVec)) {
		case 1:
			setSize(SMALL_BOARD);
			break;
		case 2:
			setSize(MEDIUM_BOARD);
			break;
		case 3:
			setSize(LARGE_BOARD);
			break;
		}
		switch (selectMenu(difficultyVec)) {
		case 1:
			setDifficulty(EASY_DIFFICULTY);
			break;
		case 2:
			setDifficulty(MEDIUM_DIFFICULTY);
			break;
		case 3:
			setDifficulty(HARD_DIFFICULTY);
			break;
		}
	}
	bool mainMenu() {
		bool continueMenu = true;
		bool startGame = true;
		HANDLE console_color;
		vector <string> menuVec{ "MAIN MENU", "Start Game", "View High Scores", "Exit" };
		while (continueMenu) {
			switch (selectMenu(menuVec)) {
			case 1:
				cout << "Starting a new game..." << endl;
				gameSettings();
				continueMenu = false;
				break;
			case 2:
				highscores.displayHighscores();
				system("pause>nul"); //wait for key
				break;
			case 3:
				cout << "Exiting game." << endl;
				continueMenu = false;
				startGame = false;
				break;
			default:
				cout << "Invalid selection, please try again." << endl;
				break;
			}
		}
		return startGame;
	}
	string recordScore(int score) {
		string keys = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		string name = "-----";
		int nameIndex = 0;
		int selection = 0;
		bool running = true;
		HANDLE console_color = GetStdHandle(STD_OUTPUT_HANDLE);

		system("cls");
		while (running) {
			gotoxy(0, 0); // prevent flickering while navigating menu
			highscores.displayHighscores();
			cout << "-----------------------------------" << endl << endl
				<< "ENTER YOUR NAME" << endl
				<< "  Name: " << name << endl
				<< " Score: " << score << endl << endl << " ";
			for (int i = 0; i < keys.size(); ++i) {
				if (i % 10 == 0 && i != 0) {
					cout << endl << " ";
				}
				if (selection == i) {
					SetConsoleTextAttribute(console_color, 23);
					cout << keys.at(i);
					SetConsoleTextAttribute(console_color, 15);
					cout << " ";
				}
				else {
					cout << keys.at(i) << " ";
				}
			}

			cout << endl << endl
				<< " Press enter to select/confirm" << endl
				<< " Press backspace to undo" << endl << endl;
			system("pause>nul"); //wait for key
			if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
				selection == 0 ? selection : selection--;
			}
			if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
				selection == keys.size() - 1 ? selection : selection++;
			}
			if (GetAsyncKeyState(VK_UP) & 0x8000) {
				selection - 10 < 0 ? selection : selection -= 10;
			}
			if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
				selection + 10 >= keys.size() ? selection : selection += 10;
			}
			if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
				if (nameIndex == 5) {
					running = false;
				}
				else {
					name.at(nameIndex) = keys.at(selection);
					nameIndex++;
				}
			}
			if (GetAsyncKeyState(VK_BACK) & 0x8000) {
				if (nameIndex > 0) {
					nameIndex--;
				}
				name.at(nameIndex) = '-';
			}
		}
		return name;
	}

	void c_update(int time, bool& game_running)
	{
		while (game_running)
		{
			if (mut.try_lock()) {
				ShowConsoleCursor(false); // rehides cursor if window is resized
				gotoxy(0, 0);
				printBoard();
				timeBonus > 1 ? --timeBonus : 1;
				mut.unlock();
			}
			this_thread::sleep_for(chrono::milliseconds(time));
		}
	}
};
#endif