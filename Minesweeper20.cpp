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