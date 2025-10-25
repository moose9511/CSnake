#include <iostream>
#include <windows.h>
#include <cctype>
#include <cstdio>
#include <chrono>
#include <vector>
#include <thread>
#include <array>
#include <random>

using namespace std;

vector<array<int, 2>> pos;
vector<array<int, 2>> dir; // x, y direction
array<int, 2> foodPos;
const int rows = 20;
const int cols = 20;

bool quit = false;
bool printing = false;

std::random_device rd; // obtain a random number from hardware
std::mt19937 gen(rd()); // seed the generator
std::uniform_int_distribution<> rowRange(1, rows-2); // define the range
std::uniform_int_distribution<> colRange(1, cols-2); // define the range

static class Direction {
public:
	static array<int, 2> tempDir;

	Direction() {
		tempDir = { 0, 0 };
	}

	static void setDir() {
		// set direction of head segment based on key input
		if ((GetAsyncKeyState('W') & 0x8000) != 0 && dir[0][1] != 1) {
			tempDir[0] = 0; tempDir[1] = -1;
		}
		else if ((GetAsyncKeyState('A') & 0x8000) != 0 && dir[0][0] != 1) {
			tempDir[0] = -1; tempDir[1] = 0;
		}
		else if ((GetAsyncKeyState('S') & 0x8000) != 0 && dir[0][1] != -1) {
			tempDir[0] = 0; tempDir[1] = 1;
		}
		else if ((GetAsyncKeyState('D') & 0x8000) != 0 && dir[0][0] != -1) {
			tempDir[0] = 1; tempDir[1] = 0;
		}
	}

};

array<int, 2> Direction::tempDir = { 0, 0 };

void startDirUpdates() {
	while (!quit) {
		if (!printing) {
			Direction::setDir();
		}
		this_thread::sleep_for(chrono::milliseconds(10));
	}
}

void placeFood() {
	// place food at random position
	foodPos[0] = colRange(gen);
	foodPos[1] = rowRange(gen);

	cout << foodPos[0] << foodPos[1];
}

// Function to print the board
bool printBoard() {
	printing = true;

	for (int i = 0; i < rows; i++) { // each row
		for (int j = 0; j < cols; j++) { // each column

			// if player hits wall, return true to quit
			if (pos[0][0] <= 0 || pos[0][0] >= cols - 1 || pos[0][1] <= 0 || pos[0][1] >= rows - 1)
				return true;

			// if head collides with food
			bool printFood = false;
			if(foodPos[0] == j && foodPos[1] == i) {
				if(pos[0][0] == foodPos[0] && pos[0][1] == foodPos[1]) {
					// grow player
					pos.push_back({ pos.back()[0] - dir.back()[0], pos.back()[1] - dir.back()[1] });
					dir.push_back({ dir.back()[0], dir.back()[1] });
					placeFood();
				}
				else {
					// print food
					cout << "()";
					printFood = true;
				}
				
			}

			// prints player segments
			bool printPlayer = false;
			if (!printFood) {
				for (int n = 0; n < pos.size(); n++) {
					if (n != 0 && pos[n][0] == pos[0][0] && pos[n][1] == pos[0][1]) {
						return true;
					} else if (pos[n][0] == j && pos[n][1] == i) {
						cout << "[]";
						printPlayer = true;
					}
				}
			}
			
			// prints board borders and empty spaces
			if (!printPlayer && !printFood) {
				if (i == 0 || i == rows - 1)
					cout << "--";
				else if (j == 0 || j == cols - 1)
					cout << "||";
				else
					cout << "  ";
			}
		}
		cout << endl;
	}

	return false;
}

int main()
{
	pos.push_back({ cols / 2, rows / 2 }); // initial position in the center
	dir.push_back({ 0, 0 }); // initial direction up
	
	placeFood();

	// set direction of head
	thread dirThread(startDirUpdates);

	// game loop
	while (!quit)
	{
		// clear screen
		system("cls");

		// print board
		quit = printBoard();
		printing = false;

		dir[0][0] = Direction::tempDir[0]; // update head direction
		dir[0][1] = Direction::tempDir[1];

		// update positions of head and segments
		for (int n = 0; n < pos.size(); n++) {
			pos[n][0] += dir[n][0];
			pos[n][1] += dir[n][1];
		}

		// set directions of trailing segments
		for (int i = pos.size() - 1; i > 0; i--) {
			dir[i][0] = dir[i - 1][0]; dir[i][1] = dir[i - 1][1];
		}

		this_thread::sleep_for(chrono::milliseconds(300));
	}

	dirThread.join();

	Sleep(10);
}