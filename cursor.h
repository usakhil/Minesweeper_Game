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
		cursorShapes = "\\|/-";
	}
	int getCursorPos() const { return cursorPos; }
	void setCursorPos(int newPos) { cursorPos = newPos; }
	char showCursor() {
		return cursorShapes.at((++shapeIndex < 4) ? shapeIndex : shapeIndex = 0);
	}
};
int Cursor::cursorPos = 0;
#endif