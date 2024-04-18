#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

struct highscore {
	int score;
	string name;
};

class Highscores {
private:
	vector<highscore> highscores;
public:
	Highscores() {
		highscore temp;
		ifstream file;
		string line;
		file.open("highscores.txt");
		if (file) {
			while (getline(file, line)) {
				stringstream(line) >> temp.score >> temp.name;
				highscores.push_back(temp);
			}
		}
		else {
			cout << "File failed to open" << endl;
		}
		file.close();
	}

	void addScore(const highscore& newScore) {
		highscores.push_back(newScore);
	}
	void updateHighscores() {
		ofstream file;
		string line;
		file.open("highscores.txt");
		if (file) {
			for (int i = 0; i < 5 && i < highscores.size(); ++i) {
				file << highscores.at(i).score << " " << highscores.at(i).name << endl;
			}
		}
		else {
			cout << "File failed to open" << endl;
		}
		file.close();
	}

	void sortHighscores() {
		for (int i = 0; i < highscores.size() - 1; ++i) {
			for (int j = i + 1; j < highscores.size(); ++j) {
				if (highscores.at(i).score < highscores.at(j).score) {
					swap(highscores.at(i), highscores.at(j));
				}
			}
		}
	}

	void displayHighscores() {
		cout << "HIGHSCORES" << endl;
		for (int i = 0; i < 5; ++i) {
			cout << " " << i + 1 << ". ";
			if (i >= highscores.size()) {
				cout << "Name: -" << endl
					<< "  Score: -" << endl << endl;
			}
			else {
				cout << "Name: " << highscores.at(i).name << endl
					<< "  Score: " << highscores.at(i).score << endl << endl;
			}
		}
	}
};

#endif