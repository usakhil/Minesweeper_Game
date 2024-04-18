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