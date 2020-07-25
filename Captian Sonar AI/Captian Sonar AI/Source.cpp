// Jared Smith
// CIS 1111
// 11/22/2019
// Final project
/* This program runs an AI for the game Captain Sonar with different type of AI  */
// Last revision 12/13/2019 10:50pm

#include <iostream>
#include <string>
#include <iomanip>
#include <thread>
#include <future>   
#include <exception>
#include <cstdlib>    
#include <ctime>
#include <time.h>
#include <windows.h> 
#include <conio.h>
#include <random>
#include <fstream> 

// Data storage variables
int mapNumber;


using namespace std;

bool mapBoundries[15][15]; // This will only hold the map boundries and no ones path
int userSonarHints[4]; // TODO Set up Radio OP use of sonar hints

int userPositionCount;
int userSurfacePosition[3]; // TODO Set up the Radio OP's use of Surface Position

string userPositions[50]; // Saves every move the user makes

int triedPosition[51][2]; // counts how long the user has gone and is the active position the Radio OP is editing

bool positionsSaved[4] = { false,false,false,false }; // Says if a position has a saved variable or not
int userPossibleLocation[4][4];// Global variable so that others can read it while only Radio OP can edit it

bool AIRadipOPTriedSpaces[15][15]; // MAY NOT BE NEEDED NOW BUT FUTURE USE

int radioCertainty = 0; // How certain the Radio AI is about where the user is
int surfaceTime = 0; // How long the Captain has to wait until his play can resume

bool AIMap[15][15]; // Saves the map boundries and the path of the AI

int userReadTries = 0; // A variable that is updated when the Radio OP is told to stop. It is the read head of the user
bool userDeadZones[15][15]; // Saves the map boundries and the path of the user (For what the Radio OP think he has gone)
int AIPath[50][2]; // Saves the AI's path 
string AICompass[30]; // Saves the AI's compass head for north, south, east, and west
int nextPath = 0; // Next write location for the AIPath


const int KEY_ARROW_CHAR1 = 224;
const int KEY_ARROW_UP = 72;
const int KEY_ARROW_DOWN = 80;
const int KEY_ARROW_LEFT = 75;
const int KEY_ARROW_RIGHT = 77;

bool XFailed[2]; // These three may be able to be put into own function. Probably should
bool YFailed[2];
bool XorY;

// engineer variables
bool shouldSurface = false;
int engNextPath = 1; // instead of using nextPath that the captain uses the engineer is special and gets his own
int engineerUses; // How many more times the engineer has to work until waiting for more work
bool moveForWant[4]; // Suggests where the captain should move to keep wants active

// first mate variables
bool firstMateListMade = false; // It's his christmas list
int firstMateUses; // How many times the first mate has to go until being done

// captain variables
bool AICalled[8];
bool wants[3]; // Wants means guns detection or silence
int activeMine = 0;
int beInRangeofUser = 0; // For personality of AI. Sometimes he just want's to be closer ok?
int AIHealth = 4;
int userTorpedoPosition[2];
int minesPosition[5][2]; // Saves captains Mines
int mineCount = 0; // Named accordingly


bool mapChosen = false; // All of these bools are for a couple of things that can't really be put in there own function
bool AISurfaced = false;// TODO Go over bools and remove what doesn't need to be there
bool AIFirstPosition = false;
bool keepUserPosition = false;
bool added;
bool writePathDone;
bool menuDone;
bool gameover;
bool movementDone;
bool AICalledStop = true;


bool gunsBroke; // These are used by the captain and the First mate and maybe somewhere else
bool detectionBroke;
bool specialBroke;

char YNInputs() { // Allows the user to enter a letter and get instant feedback
	bool givenInput = false;
	while (!givenInput) {
		if (_kbhit()) {
			switch (_getch()) {
			case 'n':
				return 'n';
				break;
			case 'y':
				return 'y';
				break;
			}
		}
	}
	return 'n';
}
struct AITypes { // This is the struct holding this whole program together. It saves the AiTypes and data about them
	int difficulty = 1;
	string type = "Easy";
	int responseTime = 20;
	bool done;

}captain, engineer, firstMate, radioOP;
bool noNoAreas(int y, int x, bool player = false, bool empty = false) { // The first function (Besides main) that I made. This function makes my life easy
	bool areaClear;
	if (y == -1 || x == -1 || y == 15 || x == 15) { // It checks if positions are within the borders they need to be in
		areaClear = false;
	}
	else if (!player && empty == true) {
		if (mapBoundries[y][x] == true) {
			areaClear = false;
		}
		else {
			areaClear = true;
		}
	}
	else if (player == false) {
		if (AIMap[y][x] == true) {
			areaClear = false;
		}
		else {
			areaClear = true;
		}
	}
	else {
		if (userDeadZones[y][x] == true) {
			areaClear = false;
		}
		else {
			areaClear = true;
		}
	}
	return areaClear;
}
int grabSectorX(int sector) { // Grabs the min X factor of a sector
	if (sector == 1 || sector == 4 || sector == 7) {
		return 0;
	}
	else if (sector == 2 || sector == 5 || sector == 8) {
		return 5;
	}
	else if (sector == 3 || sector == 6 || sector == 9) {
		return 10;
	}
}
int grabSectorY(int sector) { // Grabs the min Y factor of a sector
	if (sector == 1 || sector == 2 || sector == 3) {
		return 0;
	}
	else if (sector == 4 || sector == 5 || sector == 6) {
		return 5;
	}
	else if (sector == 7 || sector == 8 || sector == 9) {
		return 10;
	}
}
bool sectorPositionsCheck(int y, int x, int sector) { // Checks if a position is within the sector givin
	bool positionCorrect = false;
	if (sector == 1) {
		if (y >= 0 && y <= 4 && x >= 0 && x <= 4) {
			positionCorrect = true;
		}
		else {
			positionCorrect = false;
		}
	}
	else if (sector == 2) {
		if (y >= 0 && y <= 4 && x >= 5 && x <= 9) {
			positionCorrect = true;
		}
		else {
			positionCorrect = false;
		}
	}
	else if (sector == 3) {
		if (y >= 0 && y <= 4 && x >= 10 && x <= 14) {
			positionCorrect = true;
		}
		else {
			positionCorrect = false;
		}
	}
	else if (sector == 4) {
		if (y >= 5 && y <= 9 && x >= 0 && x <= 4) {
			positionCorrect = true;
		}
		else {
			positionCorrect = false;
		}
	}
	else if (sector == 5) {
		if (y >= 5 && y <= 9 && x >= 5 && x <= 9) {
			positionCorrect = true;
		}
		else {
			positionCorrect = false;
		}
	}
	else if (sector == 6) {
		if (y >= 5 && y <= 9 && x >= 10 && x <= 14) {
			positionCorrect = true;
		}
		else {
			positionCorrect = false;
		}
	}
	else if (sector == 7) {
		if (y >= 10 && y <= 14 && x >= 0 && x <= 4) {
			positionCorrect = true;
		}
		else {
			positionCorrect = false;
		}
	}
	else if (sector == 8) {
		if (y >= 10 && y <= 14 && x >= 5 && x <= 9) {
			positionCorrect = true;
		}
		else {
			positionCorrect = false;
		}
	}
	else if (sector == 9) {
		if (y >= 10 && y <= 14 && x >= 10 && x <= 14) {
			positionCorrect = true;
		}
		else {
			positionCorrect = false;
		}
	}
	else {
		cout << "\nPROBLEM AT FUNCTION SECTOR POSITION CHECK" << endl;
		system("pause");
	}
	return positionCorrect;
}
int sectorPositions(int y, int x) { // Returns the sector that those cords are in
	bool positionCorrect = false;
	
	if (y >= 0 && y <= 4 && x >= 0 && x <= 4) {
		return 1;
	}
	else if (y >= 0 && y <= 4 && x >= 5 && x <= 9) {
		return 2;
	}
	else if (y >= 0 && y <= 4 && x >= 10 && x <= 14) {
		return 3;
	}
	else if (y >= 5 && y <= 9 && x >= 0 && x <= 4) {
		return 4;
	}
	else if (y >= 5 && y <= 9 && x >= 5 && x <= 9) {
		return 5;
	}
	else if (y >= 5 && y <= 9 && x >= 10 && x <= 14) {
		return 6;
	}
	else if (y >= 10 && y <= 14 && x >= 0 && x <= 4) {
		return 7;
	}
	else if (y >= 10 && y <= 14 && x >= 5 && x <= 9) {
		return 8;
	}
	else if (y >= 10 && y <= 14 && x >= 10 && x <= 14) {
		return 9;
	}
	else {
		cout << "\nPROBLEM AT FUNCTION SECTOR POSITION CHECK" << endl;
		system("pause");
	}
}
int capPathplan[2];
int AIEasyDifficultyPath(int num, bool XY) { // There are so many bools because I couldn't find the bug in the program below and found out it was just a mistake with = instead of ==
	string compass;
	if (XY == false && YFailed[0] == true && YFailed[1] == false) {
		YFailed[1] = true;
		AICompass[nextPath] = "North";
		num--;
	}
	else if (XY == false && YFailed[1] == true) {
		YFailed[0] = true;
		AICompass[nextPath] = "South";
		num++;
	}
	else if (XY == true && XFailed[0] == true && XFailed[1] == false) {
		XFailed[1] = true;
		AICompass[nextPath] = "West";
		num--;
	}
	else if (XY == true && XFailed[1] == true) {
		XFailed[0] = true;
		AICompass[nextPath] = "East";
		num++;
	}
	else if (rand() % 2 == 0) {
		num--;
		if (XY == true) {
			XFailed[1] = true;
			AICompass[nextPath] = "West";
		}
		else {
			YFailed[1] = true;
			AICompass[nextPath] = "North";
		}
	}
	else {
		num++;
		if (XY == true) {
			XFailed[0] = true;
			AICompass[nextPath] = "East";
		}
		else {
			YFailed[0] = true;
			AICompass[nextPath] = "South";
		}
	}
	return num;
}
bool AINormalDifficultyPath() { // Does what the easy AI does but way better (kinda)
	int a = 0;					// This is the last thing I worked on as of 12/13/2019 9:05pm
	bool withinRange[2] = { false,false };
	bool positionDone = false;
	bool compass[4] = { false, false, false, false };
	int x = capPathplan[1]; int y = capPathplan[0];
	int num = 0;
	if (radioCertainty >= 5) { // If the radio OP is certain that the user is at one of the two locations then the Captain will try to get closer to them
		if (AIPath[nextPath - 1][0] - userPossibleLocation[0][2] >= beInRangeofUser && // At least that's how I think it works *shrug*
			AIPath[nextPath - 1][0] - userPossibleLocation[0][2] <= -beInRangeofUser ||
			AIPath[nextPath - 1][1] - userPossibleLocation[0][3] >= beInRangeofUser &&
			AIPath[nextPath - 1][1] - userPossibleLocation[0][3] <= -beInRangeofUser) {
			withinRange[0] = true;
		}
		if (AIPath[nextPath - 1][0] - userPossibleLocation[1][2] >= beInRangeofUser &&
			AIPath[nextPath - 1][0] - userPossibleLocation[1][2] <= -beInRangeofUser ||
			AIPath[nextPath - 1][1] - userPossibleLocation[1][3] >= beInRangeofUser &&
			AIPath[nextPath - 1][1] - userPossibleLocation[1][3] <= -beInRangeofUser) {
			withinRange[0] = true;
		}
		if (withinRange[0] == true && withinRange[1] == true){
			num = rand() % 2;
		}
		else if (withinRange[0] == true) { num = 0; }
		else if (withinRange[1] == true) { num = 1; }
		else { num = 2; }
		if (num != 2) {

		}
		else {
			num = rand() % 2;
		}
		do { // Moves closer to the cord that is closest to it (if able to) 
			if (!compass[0] || !compass[1] || !compass[2] || !compass[3] &&
				AIPath[nextPath - 1][0] - userPossibleLocation[num][2] >= beInRangeofUser &&
				AIPath[nextPath - 1][0] - userPossibleLocation[num][2] <= -beInRangeofUser) {
				if (!compass[0]) { y--; AICompass[nextPath] = "North"; compass[0] = true; a = 0; }
				else if (!compass[1]){ y++; AICompass[nextPath] = "South"; compass[1] = true; a = 1;}
				else if (!compass[2]) { x--; AICompass[nextPath] = "West"; compass[2] = true; a = 2;}
				else { x++; AICompass[nextPath] = "East"; compass[3] = true; a = 3;}
			}
			else if (!compass[0] || !compass[1] || !compass[2] || !compass[3] &&
				AIPath[nextPath - 1][1] - userPossibleLocation[num][3] >= beInRangeofUser &&
				AIPath[nextPath - 1][1] - userPossibleLocation[num][3] <= -beInRangeofUser) {
				if (!compass[2]) { x--; AICompass[nextPath] = "West"; compass[2] = true; a = 0;}
				else if (!compass[3]) { x++; AICompass[nextPath] = "East"; compass[3] = true; a = 1;}
				else if (!compass[0]) { y--; AICompass[nextPath] = "North"; compass[0] = true; a = 2;}
				else { y++; AICompass[nextPath] = "South"; compass[1] = true; a = 3;}
			}
			else { // If all fail give the error code 16
				AICompass[nextPath] = "";
				y = 16;
				x = 0;
				break;
			}

			positionDone = noNoAreas(y,x);
			if (!positionDone) { // If the position failed reset back to last cord
				if (compass[0] == true && a == 0) { y++; }
				else if (compass[1] == true && a == 1) { y--; }
				else if (compass[2] == true && a == 2) { x++; }
				else if (compass[3] == true && a == 3) { x--; }
			}
		} while (!positionDone);
		
	}
	else { // if Radio OP is not sure where the heck the user is drive around randomly
		if (moveForWant[3] == true) { // If engineer says move east try to move east
			x++;
			positionDone = noNoAreas(y, x);
			if (!positionDone) {
				x--;
			}
			else {
				AICompass[nextPath] = "East";
			}
		}
		if (moveForWant[0] == true && !positionDone) { // If engineer says move north and last failed try to move north
			y--;
			positionDone = noNoAreas(y, x); // TODO See if engineer says to move works right
			if (!positionDone) {
				y++;
			}
			else {
				AICompass[nextPath] = "North";
			}
		}
		if (moveForWant[1] == true && !positionDone) { // If engineer says move south and last failed try to move south
			y++;
			positionDone = noNoAreas(y, x);
			if (!positionDone) {
				y--;
			}
			else {
				AICompass[nextPath] = "South";
			}
		}
		if (moveForWant[2] == true && !positionDone) { // If engineer says move west and last failed try to move west
			x--;
			positionDone = noNoAreas(y, x);
			if (!positionDone) {
				x++;
			}
			else {
				AICompass[nextPath] = "West";
			}
		}
		else {
			int num = 0;
			do {
				if (rand() % 2 == 0 || XFailed[0] == true && XFailed[1] == true) {
					if (YFailed[0] == false || YFailed[1] == false) {
						num = rand() % 2;
						if (num == 0) { y++; AICompass[nextPath] = "South"; YFailed[0] = true; a = 1; }
						else { y--; AICompass[nextPath] = "North"; YFailed[1] = true; a = 0;}
					}
				}
				else if (XFailed[0] == false || XFailed[1] == false) {
					num = rand() % 2;
					if (num == 0) { x++; AICompass[nextPath] = "East"; XFailed[0] = true; a = 3;}
					else { x--; AICompass[nextPath] = "West"; XFailed[1] = true; a = 2;}
				}
				else {// If all fail give the error code 16
					AICompass[nextPath] = "";
					y = 16;
					x = 0;
					break;
				}
				if (!positionDone) {
					if (YFailed[1] == true && a == 0) { y++; }
					else if (YFailed[0] == true && a == 1) { y--; }
					else if (XFailed[1] == true && a == 2) { x++; }
					else if (XFailed[0] == true && a == 3) { x--; }
				}
				positionDone = noNoAreas(y, x);
			} while (!positionDone);
		}
	}
	capPathplan[1] = x; capPathplan[0] = y;
	return positionDone;
}
void AIHardDifficultyPath() { // TODO Set up hard captain path
	
}
void writePath(int preY, int preX) { // Writes the path of the captain dependent on how able he is at doing his job
	bool positionDone = false;
	float AIYPosition;
	float AIXPosition;
	int AIPrePosition;
	int x = preX;
	int y = preY;
	if (!writePathDone) {
		AIMap[y][x] = true;
		do {
			if (captain.difficulty == 1) { // This version of the captain likes to go in squares and get himself stuck
				if (rand() % 2 == 0) {
					if (YFailed[0] == false || YFailed[1] == false) {
						AIPrePosition = y;
						y = AIEasyDifficultyPath(y, false);
						XorY = false;
					}
				}
				else if (XFailed[0] == false || XFailed[1] == false) {
					AIPrePosition = x;
					x = AIEasyDifficultyPath(x, true);
					XorY = true;
				}
				else { // Error code 16 for being stuck
					AIPath[nextPath][0] = 16;
					break;
				}
				positionDone = noNoAreas(y, x);
				if (!positionDone) { // If false reset position and try again until realizing fighting is futile
					if (XorY == true) {
						x = AIPrePosition;
					}
					else if (XorY == false) {
						y = AIPrePosition;
					}
					else {
						cout << "\n\nERROR AT XorY CALC" << endl;
						system("pause");
					}
				}
			}
			else if (captain.difficulty == 2) { // I have faith in this ones ability, not all things have been tested. Could somehow end up in a loop or not post position

				if (YFailed[0] == false || YFailed[1] == false || XFailed[0] == false || XFailed[1] == false) {
					capPathplan[0] = AIPath[nextPath - 1][0];
					capPathplan[1] = AIPath[nextPath - 1][1];
					positionDone = AINormalDifficultyPath(); // Everything is done in that function
					x = capPathplan[1];
					y = capPathplan[0];
				}
				else {// TODO Clean up Normal captain code
					AIPath[nextPath][0] = 16;
					break;
				}
			}
			else if (captain.difficulty == 3) {
				AIHardDifficultyPath();
				// TODO Set up captain difficulty hard
			}
		} while (!positionDone);
		if (positionDone == true) { // If position is true then log it
			AIPath[nextPath][0] = y; AIPath[nextPath][1] = x;
			AIMap[y][x] = true;
			XFailed[0] = false; XFailed[1] = false;
			YFailed[0] = false; YFailed[1] = false;
			writePathDone = true;
		}
		else {
			XFailed[0] = false; XFailed[1] = false;
			YFailed[0] = false; YFailed[1] = false;
			writePathDone = false;
		}
	}
}

int userCompassCounts[4]; // All of these variables were made for the Radio OP
int userDroneCheckPoss[3];
int silenceReadTries = 0;
int YSOfSectorCheck = 0;
int XSOfSectorCheck = 0;
bool positionsTried[15][15];
int sectorTrying = 10;
bool userSectorStarters[9];

void radioOPNormal(int readTries) { // Executes if the Radio OP is normal
	bool finished = false;
	int XsectorMIN = 0;
	int YsectorMIN = 0;
	int userX = 0;
	int userY = 0;
	bool positionDone = false;
	int num = 0;
	if (userCompassCounts[0] >= 4 && readTries <= 6) { // if the user moves north 5 times with only moving 7 times total then can't be in sector 1,2,3
		userSectorStarters[0] = true; 
		userSectorStarters[1] = true;
		userSectorStarters[2] = true;
		if (sectorTrying == 0 || sectorTrying == 1 || sectorTrying == 2) {
			sectorTrying = 10;
		}
	}
	if (userCompassCounts[1] >= 4 && readTries <= 6) {// if the user moves south 5 times with only moving 7 times total then can't be in sector 7,8,9
		userSectorStarters[6] = true;
		userSectorStarters[7] = true;
		userSectorStarters[8] = true;
		if (sectorTrying == 6 || sectorTrying == 7 || sectorTrying == 8) {
			sectorTrying = 10;
		}
	}
	if (userCompassCounts[2] >= 4 && readTries <= 6) {// if the user moves east 5 times with only moving 7 times total then can't be in sector 3,6,9
		userSectorStarters[2] = true;
		userSectorStarters[5] = true;
		userSectorStarters[8] = true;
		if (sectorTrying == 2 || sectorTrying == 5 || sectorTrying == 8) { // TODO Clean up code for Radio OP Normal
			sectorTrying = 10;
		}
	}
	if (userCompassCounts[3] >= 4 && readTries <= 6) {// if the user moves west 5 times with only moving 7 times total then can't be in sector 1,4,7
		userSectorStarters[0] = true;
		userSectorStarters[3] = true;
		userSectorStarters[6] = true;
		if (sectorTrying == 0 || sectorTrying == 3 || sectorTrying == 6) {
			sectorTrying = 10;
		}
	}
	while (!finished) {
		while (sectorTrying == 10) { // if sectorTrying = 10 then try a new sector cause that sector doesn't exist
			num = rand() % 9;
			if (!userSectorStarters[num]) {
				sectorTrying = num;
				cout << "Trying Sector " << num + 1 << endl;
				userSectorStarters[num] = true;
				positionDone = false;
				YSOfSectorCheck = 0;
				XSOfSectorCheck = 0;
			}
		} // Grab the X and Y min of the sector
		XsectorMIN = grabSectorX(sectorTrying + 1);
		YsectorMIN = grabSectorY(sectorTrying + 1);
		while (!positionDone) {
			if (!positionDone) {
				while (YSOfSectorCheck < 5) { // All sectors are 5 by 5 so this only runs through 25 positions and tries all of them
					positionDone = true;
					while (XSOfSectorCheck < 5) {
						if (positionsTried[YSOfSectorCheck + YsectorMIN][XSOfSectorCheck + XsectorMIN] == false) {
							userX = XSOfSectorCheck + XsectorMIN;
							userY = YSOfSectorCheck + YsectorMIN;
							positionDone = false;
							break;
						}
						else {
							XSOfSectorCheck++;
						}
					}
					if (positionDone == false) {
						positionDone = noNoAreas(userY, userX, true);
						positionsTried[userY][userX] = true;
						if (positionDone == true) {   // TODO Check if Radio OP Normal code works right. 12/13/2019 3:25am checked. It should
							triedPosition[0][0] = userY;
							triedPosition[0][1] = userX;
							userDeadZones[userY][userX] = true;  // Makes the location not allowed to be entered
							finished = true;
							break;
						}
					}
					else {
						YSOfSectorCheck++;
						XSOfSectorCheck = 0;
					}
					if (YSOfSectorCheck == 5 && XSOfSectorCheck == 0) { // If All 25 are tried try sector 10
						positionDone = true; sectorTrying = 10;
						break;
					}
				}
			}
		}
	}
}
void AIRadioOP() {  // The big brain. Calculating where the user is
	bool tried[3] = { false,false,false };
	int activeTry = 0;
	int checkCount = 0;
	int possibleCount = 0;
	bool firstPosiDone = false;
	bool positionDone = false;
	bool noMore = false;
	do {
		srand(time(NULL) - 10000);
		int readTries = userReadTries;
		int userX = triedPosition[readTries][1];
		int userY = triedPosition[readTries][0];
		bool done = false;
		while (!movementDone) {
			if (radioOP.difficulty == 1) {
				chrono::milliseconds dura1((rand() % 1 + radioOP.responseTime) * 100);
				this_thread::sleep_for(dura1);
				if (keepUserPosition == true) {
					firstPosiDone = true;
					positionDone = true;
					keepUserPosition = false;
				}
				if (!firstPosiDone) {
					firstPosiDone = true;
					for (int i = 0; i <= readTries; i++) { // Clears all positions from where the AI thought the player was
						int x = 0; int y = 0;
						x = triedPosition[i][1]; y = triedPosition[i][0];
						triedPosition[i][0] = 0; triedPosition[i][1] = 0;
						userDeadZones[y][x] = false;
					}
					readTries = 0;
					do { // Tries a new location
						userX = rand() % 15;
						userY = rand() % 15;

						positionDone = noNoAreas(userY, userX, true);
					} while (!positionDone);
					triedPosition[0][1] = userX; triedPosition[0][0] = userY; // Saves location in first slot of a list
					userDeadZones[userY][userX] = true; // Makes the location not allowed to be entered

				}
				while (userPositions[readTries] != "" && positionDone == true) { // If player inputs a move this checks if that new location is safe and if so saves it
					if (userPositions[readTries] == "North") {
						userY--;

						positionDone = noNoAreas(userY, userX, true);
					}
					else if (userPositions[readTries] == "South") {
						userY++;

						positionDone = noNoAreas(userY, userX, true);
					}
					else if (userPositions[readTries] == "East") {
						userX++;

						positionDone = noNoAreas(userY, userX, true);
					}
					else if (userPositions[readTries] == "West") {
						userX--;

						positionDone = noNoAreas(userY, userX, true);
					}
					if (positionDone == true) { // If the location is clear then log it
						userDeadZones[userY][userX] = true;
						readTries++;
						triedPosition[readTries][1] = userX; triedPosition[readTries][0] = userY;
					}
				}
				if (!positionDone) {
					firstPosiDone = false;
				}
			}
			else if (radioOP.difficulty == 2) {
				checkCount = 2;
				while (!movementDone) {
					chrono::milliseconds dura1((rand() % 1 + radioOP.responseTime) * 100);
					this_thread::sleep_for(dura1);
					for (int i = 0; i <= readTries; i++) { // Clears all positions from where the AI thought the player was
						int x = 0; int y = 0;
						x = triedPosition[i][1]; y = triedPosition[i][0];
						triedPosition[i][0] = 0; triedPosition[i][1] = 0;
						userDeadZones[y][x] = false;
					}
					if (positionsSaved[0] == true && positionsSaved[1] == true && positionsSaved[2] == true) {
						noMore = true;
					}
					if (positionsSaved[0] == true && noMore == true && !tried[0]) { // If not trying random try position Saved 0
						possibleCount = 0; positionDone = true;
						triedPosition[0][0] = userPossibleLocation[0][0];
						triedPosition[0][1] = userPossibleLocation[0][1];
						userDroneCheckPoss[0] = userPossibleLocation[1][2];
						userDroneCheckPoss[1] = userPossibleLocation[1][3];
						userDroneCheckPoss[2] = 1; // Sets drone position to possible position 1
						tried[0] = true;
						radioCertainty++;
					}
					else if (positionsSaved[1] == true && noMore == true && !tried[1]) {// If just tried position saved 0 try position 1
						possibleCount = 1; positionDone = true;
						triedPosition[0][0] = userPossibleLocation[1][0];
						triedPosition[0][1] = userPossibleLocation[1][1];
						userDroneCheckPoss[0] = userPossibleLocation[0][2];
						userDroneCheckPoss[1] = userPossibleLocation[0][3];
						userDroneCheckPoss[2] = 0; // Sets drone position to possible position 0
						tried[0] = false; tried[1] = false;
						radioCertainty++;
					}
					if (!firstPosiDone) {
						firstPosiDone = true;
						if (!noMore) { // No more random if no more positions are allowed to try or position saved is full
							for (int i = 0; i < 9; i++) {
								if (userSectorStarters[i] == false) {
									radioOPNormal(readTries);
									positionDone = true;
									userX = triedPosition[0][1];
									userY = triedPosition[0][0]; i = 10;
									break;
								}
								else {
									noMore = true;
								}
							}
						}
					}
					positionDone = true;
					readTries = 0;
					while (userPositions[readTries] != "" && positionDone == true) { // If player inputs a move this checks if that new location is safe and if so saves it
						if (userPositions[readTries] == "North") {
							userCompassCounts[0]++;
							userY--;

							positionDone = noNoAreas(userY, userX, true);
						}
						else if (userPositions[readTries] == "South") {
							userCompassCounts[1]++;
							userY++;

							positionDone = noNoAreas(userY, userX, true);
						}
						else if (userPositions[readTries] == "East") {
							userCompassCounts[2]++;
							userX++;

							positionDone = noNoAreas(userY, userX, true);
						}
						else if (userPositions[readTries] == "West") {
							userCompassCounts[3]++;
							userX--;

							positionDone = noNoAreas(userY, userX, true);
						}
						if (positionDone == true) { // If the location is clear then log it
							userDeadZones[userY][userX] = true;
							readTries++;
							triedPosition[readTries][1] = userX; triedPosition[readTries][0] = userY;
						}
						if (readTries == silenceReadTries) {
							// TODO figure out math for silence RadioOP diff 2
						}
					}
					if (readTries > 0 && positionDone == true && !noMore) { // Starts the process of checking many locations instead of just one
						if (possibleCount >= checkCount) {
							possibleCount = 0;
						}
						radioCertainty++;
						userPossibleLocation[possibleCount][0] = triedPosition[0][0]; // Saves the starting possible location
						userPossibleLocation[possibleCount][1] = triedPosition[0][1];
						userPossibleLocation[possibleCount][2] = triedPosition[readTries][0]; // Saves end location
						userPossibleLocation[possibleCount][3] = triedPosition[readTries][1];
						positionsSaved[possibleCount] = true;
						possibleCount++;
						positionDone = false;
					}
					else if (!positionDone) {
						for (int i = 0; i < 2; i++) {
							if (triedPosition[0][0] == userPossibleLocation[i][0] && triedPosition[0][1] == userPossibleLocation[i][1]) {
								userPossibleLocation[i][0] = 0; userPossibleLocation[i][1] = 0; positionsSaved[i] = false;
								userPossibleLocation[i][2] = 0; userPossibleLocation[i][3] = 0;
								possibleCount = i; noMore = false; radioCertainty = 0;
							}
						}
					}
					if (readTries > 0) { firstPosiDone = false; }
				}
			}
			else if (radioOP.difficulty == 3) {
				// TODO Set up Radio OP difficulty hard
			}
		} 
		userReadTries = readTries;
		keepUserPosition = firstPosiDone;
		chrono::seconds set(1);
		this_thread::sleep_for(set);
	} while (!gameover);
}
struct equipment { // Holds all the equipment for first mate. Allows the user to change maxes
	int max;
	int current = 0;
}mines, torpedo, drones, sonar, silence, scenario;
int firstMateList[5];
void AIFirstMate() {
	int slotPosition = 0;
	int personality = 0;
	int failedTries = 0;
	do {
		firstMate.done = true;
		srand(time(NULL) + 100);
		if (firstMate.difficulty == 1) { // Goes if first Mate difficulty is Easy
			while (!movementDone) {
				chrono::milliseconds dura1((rand() % 30 + 1 + firstMate.responseTime) * 100);
				this_thread::sleep_for(dura1);

				if (firstMateUses > 0) {
					int num;
					num = rand() % 5; // Sets random guages up by one
					if (num == 0 && mines.current < mines.max) {
						mines.current++;
						if (mines.current == mines.max) {
							cout << "AI Mines ready!" << endl;
						}
						firstMateUses--;
					}
					else if (num == 1 && torpedo.current < torpedo.max) {
						torpedo.current++;
						if (torpedo.current == torpedo.max) {
							cout << "AI Torpedo ready!" << endl;
						}
						firstMateUses--;
					}
					else if (num == 2 && drones.current < drones.max) {
						drones.current++;
						if (drones.current == drones.max) {
							cout << "AI Drones ready!" << endl;
						}
						firstMateUses--;
					}
					else if (num == 3 && sonar.current < sonar.max) {
						sonar.current++;
						if (sonar.current == sonar.max) {
							cout << "AI Sonar ready!" << endl;
						}
						firstMateUses--;
					}
					else if (num == 4 && silence.current < silence.max) {
						silence.current++;
						if (silence.current == silence.max) {
							cout << "AI Silence ready!" << endl;
						}
						firstMateUses--;
					}
					else if (num == 5 && scenario.current < scenario.max) { // TODO Set up scenario
						scenario.current++;
						if (scenario.current == scenario.max) {
							cout << "AI Scenario ready!" << endl;
						}
						firstMateUses--;
					}
					failedTries++;
					if (failedTries >= 8) { firstMateUses = 0; failedTries = 0; } // If all slots are full say you are done
				}
				if (firstMateUses == 0) {  // Tells the captain that they are done
					failedTries = 0;
					firstMate.done = true;
				}
			}
		}
		else if (firstMate.difficulty == 2) { // Goes if first Mate difficulty is Normal
			chrono::milliseconds dura1((rand() % 30 + 1 + firstMate.responseTime) * 100);
			this_thread::sleep_for(dura1);
			if (captain.difficulty == 1 && !firstMateListMade) { // checking if the captain is difficulty Easy, if so will set persoality itselff
				personality = rand() % 3;
				switch (personality) { // TODO Tweak settings with changing personalities
				case 1: firstMateList[0] = 0; firstMateList[1] = 1;
					firstMateList[2] = 4; firstMateList[3] = 2;
					firstMateList[4] = 3; break;
				case 2: firstMateList[0] = 2; firstMateList[1] = 3;
					firstMateList[2] = 1; firstMateList[3] = 4;
					firstMateList[4] = 0; break;
				case 3: firstMateList[0] = 2; firstMateList[1] = 4;
					firstMateList[2] = 1; firstMateList[3] = 3;
					firstMateList[4] = 0; break;
				}
				firstMateListMade = true;
			}
			if (firstMateUses > 0) {
				int num;
				num = firstMateList[slotPosition]; // Fills up the next gauge in the slot until full
				if (num == 0 && mines.current < mines.max) {
					mines.current++;
					if (mines.current == mines.max) {
						cout << "AI Mines ready!" << endl;
						slotPosition++;
					}
					firstMateUses--;
				}
				else if (num == 1 && torpedo.current < torpedo.max) {
					torpedo.current++;
					if (torpedo.current == torpedo.max) {
						cout << "AI Torpedo ready!" << endl;
						slotPosition++;
					}
					firstMateUses--;
				}
				else if (num == 2 && drones.current < drones.max) {
					drones.current++;
					if (drones.current == drones.max) {
						cout << "AI Drones ready!" << endl;
						slotPosition++;
					}
					firstMateUses--;
				}
				else if (num == 3 && sonar.current < sonar.max) {
					sonar.current++;
					if (sonar.current == sonar.max) {
						cout << "AI Sonar ready!" << endl;
						slotPosition++;
					}
					firstMateUses--;
				}
				else if (num == 4 && silence.current < silence.max) {
					silence.current++;
					if (silence.current == silence.max) {
						cout << "AI Silence ready!" << endl;
						slotPosition++;
					}
					firstMateUses--;
				}
				else if (num == 5 && scenario.current < scenario.max) { // TODO Set up scenario
					scenario.current++;
					if (scenario.current == scenario.max) {
						cout << "AI Scenario ready!" << endl;
						slotPosition++;
					}
					firstMateUses--;
				}
				if (slotPosition == 6) { slotPosition = 0; firstMateListMade = false; }
			}
			if (firstMateUses == 0) {  // Tells the captain that they are done
				firstMate.done = true;
			}
		
		}
		else if (firstMate.difficulty == 3) {
			chrono::milliseconds dura1((rand() % 30 + 1 + firstMate.responseTime) * 100);
			this_thread::sleep_for(dura1);
			// TODO Set up First Mate difficulty hard
		}
		chrono::seconds set(1);
		this_thread::sleep_for(set);
	} while (!gameover);
}
bool north[6];
bool south[6];
bool east[6];
bool west[6];
void EngineerCheck() { // This checks what's broken on the ship
	if (west[0] == true || north[1] == true || north[4] == true || south[2] == true || south[3] == true || east[2] == true) { // All gun locations
		gunsBroke = true;
	}
	if (west[2] == true || west[3] == true || north[3] == true || south[0] == true || east[0] == true || east[4] == true) {// All detection locations
		detectionBroke = true;
	}
	if (west[1] == true || north[0] == true || north[2] == true || south[1] == true || south[5] == true || east[1] == true) {// All special locations
		specialBroke = true;
	}
	if (west[0] == true && west[1] == true && west[2] == true && east[2] == true) { // Yellow line
		west[0] = false; 
		west[1] = false;
		west[2] = false;
		east[2] = false;
	}
	if (north[0] == true && north[1] == true && north[2] == true && east[0] == true) { // Orange line
		north[0] = false;
		north[1] = false;
		north[2] = false;
		east[0] = false;
	}
	if (south[0] == true && south[1] == true && south[2] == true && east[1] == true) { // Grey line
		west[0] = false;
		west[1] = false;
		west[2] = false;
		east[1] = false;
	}
	if (west[0] == true && west[1] == true && west[2] == true && west[3] == true && west[4] == true && west[5] == true) { // West whole section
		west[0] = false;
		west[1] = false;
		west[2] = false;
		west[3] = false;
		west[4] = false;
		west[5] = false;
		AIHealth--;
	}
	if (north[0] == true && north[1] == true && north[2] == true && north[3] == true && north[4] == true && north[5] == true) {// North whole section
		north[0] = false;
		north[1] = false;
		north[2] = false;
		north[3] = false;
		north[4] = false;
		north[5] = false;
		AIHealth--;
	}
	if (south[0] == true && south[1] == true && south[2] == true && south[3] == true && south[4] == true && south[5] == true) {// South whole section
		south[0] = false;
		south[1] = false;
		south[2] = false;
		south[3] = false;
		south[4] = false;
		south[5] = false;
		AIHealth--;
	}
	if (east[0] == true && east[1] == true && east[2] == true && east[3] == true && east[4] == true && east[5] == true) {// East whole section
		east[0] = false;
		east[1] = false;
		east[2] = false;
		east[3] = false;
		east[4] = false;
		east[5] = false;
		AIHealth--;
	}
	if (west[4] == true && west[5] == true && north[6] == true && south[4] == true && east[3] == true && east[5] == true) {// Reactors
		west[4] = false;
		west[5] = false;
		west[6] = false;
		south[4] = false;
		east[3] = false;
		east[5] = false;
		AIHealth--;
	}
}
bool easyEngineerSurface() { // Says for easy but used for both normal and easy ATM. More testing needed with normal AI
	if (west[3] == true || north[3] == true || north[4] == true || south[3] == true || south[5] == true || east[4] == true) {
		return true;
	}
	return false;
}
void AIEngineer() {
	do {
		engineer.done = true;
		int num = 0;
		srand(time(NULL) - 1000000);
		if (engineer.difficulty == 1) { // Runs instructions if engineer Easy
			while (!movementDone) {
				chrono::milliseconds dura1((rand() % 10 + 1 + engineer.responseTime) * 100);
				this_thread::sleep_for(dura1);
				if (engineerUses > 0) {
					if (AICompass[engNextPath] == "North") { // Marks off the North section of the ship by left to right
						do {
							if (north[0] == false) { num = 0; }
							else if (north[1] == false) { num = 1;}
							else if (north[2] == false) { num = 2; }
							else { num = rand() % 6; }
							for (int i = 0; i < 6; i++) {
								if (num == i && !north[i]) {
									north[i] = true;
									num = 6;
								}
							}
						} while (num < 6);
					}
					else if (AICompass[engNextPath] == "South") {// Marks off the South section of the ship by left to right
						do {
							if (south[0] == false) { num = 0; }
							else if (south[1] == false) { num = 1; }
							else if (south[2] == false) { num = 2; }
							else { num = rand() % 6; }
							for (int i = 0; i < 6; i++) {
								if (num == i && !south[i]) {
									south[i] = true;
									num = 6;
								}
							}
						} while (num < 6);
					}
					else if (AICompass[engNextPath] == "East") {// Marks off the East section of the ship by left to right
						do {
							if (east[0] == false) { num = 0; }
							else if (east[1] == false) { num = 1; }
							else if (east[2] == false) { num = 2; }
							else { num = rand() % 6; }
							for (int i = 0; i < 6; i++) {
								if (num == i && !east[i]) {
									east[i] = true;
									num = 6;
								}
							}
						} while (num < 6);
					}
					else if (AICompass[engNextPath] == "West") {// Marks off the West section of the ship by left to right
						do {
							if (west[0] == false) { num = 0; }
							else if (west[1] == false) { num = 1; }
							else if (west[2] == false) { num = 2; }
							else { num = rand() % 6; }
							for (int i = 0; i < 6; i++) {
								if (num == i && !west[i]) {
									west[i] = true;
									num = 6;
								}
							}
						} while (num < 6);
					}
					engineer.done = false;
					engNextPath++;
					engineerUses--;
				}
				if (engineerUses == 0) { // Tells the captain that they are done
					shouldSurface = easyEngineerSurface();
					engineer.done = true;
				}
				EngineerCheck();
			}
		}
		else if (engineer.difficulty == 2) { // Runs instructions if engineer Normal
			chrono::milliseconds dura1((rand() % 30 + 1 + engineer.responseTime) * 100); // Since this is the normal version it has more direct instructions with how to keep the ship from being broken fully
			this_thread::sleep_for(dura1);												 // If the captain is normal or higher it will not mark things that the captain wants alive
			if (engineerUses > 0) {														 // It knows this from the wants array. The captain will set that. If the captain is easy it won't affect how the engineer works
				if (AICompass[engNextPath] == "North") {								 // The engineer will almost act like easy but with a couple exceptions
					do {
						if (north[0] == false && !wants[2]) { num = 0; if (wants[2] == true) { moveForWant[3] = true; } }
						else if (north[1] == false && !wants[0]) { num = 1; if (wants[0] == true) { moveForWant[3] = true; } }
						else if (north[2] == false && !wants[2]) { num = 2; if (wants[2] == true) { moveForWant[3] = true; } }
						else if (north[5] == false) { num = 5; }
						else if (north[3] == false && !wants[1]) { num = 3; }
						else if (north[4] == false && !wants[0]) { num = 4; }
						else if (wants[0] == true && !north[1] || !north[4]) { if (!north[1]) { num = 1; } else { num = 4; } }
						else if (wants[2] == true && !north[0] || !north[2]) { if (!north[0]) { num = 0; } else { num = 2; } }
						else if (wants[1] == true && !north[3]) { num = 3; }
						for (int i = 0; i < 6; i++) {
							if (num == i && !north[i]) {
								north[i] = true;
								num = 6;
							}
						}
					} while (num < 6);
				}
				else if (AICompass[engNextPath] == "South") {
					do {
						if (south[0] == false && !wants[1]) { num = 0; if (wants[1] == true) { moveForWant[3] = true; } }
						else if (south[1] == false && !wants[2]) { num = 1; if (wants[2] == true) { moveForWant[3] = true; } }
						else if (south[2] == false && !wants[0]) { num = 2; if (wants[0] == true) { moveForWant[3] = true; } }
						else if (south[4] == false) { num = 4; }
						else if (south[3] == false && !wants[0]) { num = 3; }
						else if (south[5] == false && !wants[2]) { num = 5; }
						else if (wants[0] == true && !south[2] || !south[3]) { if (!south[2]) { num = 2; } else { num = 3; } }
						else if (wants[2] == true && !south[1] || !south[5]) { if (!south[1]) { num = 1; } else { num = 5; } }
						else if (wants[1] == true && !south[0]) { num = 0; }
						for (int i = 0; i < 6; i++) {
							if (num == i && !south[i]) {
								south[i] = true;
								num = 6;
							}
						}
					} while (num < 6);
				}
				else if (AICompass[engNextPath] == "East") {
					do {
						if (east[0] == false && !wants[1]) { num = 0; if (wants[1] == true) { moveForWant[0] = true; } }
						else if (east[1] == false && !wants[2]) { num = 1; if (wants[2] == true) { moveForWant[1] = true; } }
						else if (east[2] == false && !wants[0]) { num = 2; if (wants[0] == true) { moveForWant[2] = true; } }
						else if (east[3] == false) { num = 3; }
						else if (east[4] == false && !wants[1]) { num = 4; }
						else if (wants[1] == true && !east[0] || !east[4]) { if (!east[0]) { num = 0; } else { num = 4; } }
						else if (wants[2] == true && !east[1]) { num = 1; }
						else if (wants[0] == true && !east[2]) { num = 2; }
						else if (east[5] == false) { num = 5; }
						else { num = rand() % 6; }
						for (int i = 0; i < 6; i++) {
							if (num == i && !east[i]) {
								east[i] = true;
								num = 6;
							}
						}
					} while (num < 6);
				}
				else if (AICompass[engNextPath] == "West") {
					do {
						if (west[0] == false && !wants[0]) { num = 0; if (wants[0] == true) { moveForWant[3] = true; } }
						else if (west[1] == false && !wants[2]) { num = 1; if (wants[2] == true) { moveForWant[3] = true; } }
						else if (west[2] == false && !wants[1]) { num = 2; if (wants[1] == true) { moveForWant[3] = true; } }
						else if (west[3] == false && !wants[1]) { num = 3; }
						else if (west[4] == false) { num = 4; }
						else if (wants[1] == true && !west[2] || !west[3]) { if (!west[2]) { num = 2; } else { num = 3; } }
						else if (wants[2] == true && !west[1]) { num = 1; }
						else if (wants[0] == true && !west[0]) { num = 0; }
						else if (west[5] == false) { num = 5; }
						else { num = rand() % 6; }
						for (int i = 0; i < 6; i++) {
							if (num == i && !west[i]) {
								west[i] = true;
								num = 6;
							}
						}
					} while (num < 6);
				}
				engineer.done = false;
				engNextPath++;
				engineerUses--;
			}
			if (engineerUses == 0) { // Tells the captain that they are done
				shouldSurface = easyEngineerSurface();
				engineer.done = true;
			}
			EngineerCheck();
		}
		else if (engineer.difficulty == 3) {
			chrono::milliseconds dura1((rand() % 30 + 1 + engineer.responseTime) * 100);
			this_thread::sleep_for(dura1);
			// TODO Set up Engineer difficulty hard
		}
		chrono::seconds set(1);
		this_thread::sleep_for(set);
	} while (!gameover);
}
bool torpedoCheck(int startY, int startX, int endY, int endX) { // Checks if a torpedo can reach the user without going through land
	bool positionDone = false;
	bool done = false;
	bool cantDo = false;
	int count = 0;
	int runTime = 0;
	int x = startX;
	int y = startY;
	bool compass[4] = { false, false, false, false };
	while (!done) {
		if (cantDo == true) {
			cantDo = false;
			runTime++;
			x = startX;
			y = startY;
		}
		do {
			if		(!compass[0]) { compass[0] = true; y--; positionDone = noNoAreas(y, x, false, true); }
			else if (!compass[1]) { compass[1] = true; x++; positionDone = noNoAreas(y, x, false, true); }
			else if (!compass[2]) { compass[2] = true; y++; positionDone = noNoAreas(y, x, false, true); }
			else if (!compass[3]) { compass[3] = true; x--; positionDone = noNoAreas(y, x, false, true); }
			if (positionDone == true) {
				count++; for (int i = 0; i < 4; i++) { compass[i] = false; }
				if (x == endX && y == endY) {
					return true;
				}
				if (count >= 4) {
					cantDo = true;
				}
			}
			else {
				if (compass[0] == true) { y++; }	else if (compass[1] == true) { x--; }
				else if (compass[2] == true) { y--; }	else if (compass[3] == true) { x++; }
			}
			if (compass[0] == true && compass[1] == true && compass[2] == true && compass[3] == true) { cantDo = true; }
		} while (!cantDo);
		if (runTime >= 4) {
			done = true;
		}
	}
	return false;
}
void doSurface() { // Triggers when the AI surfaces (STILL TESTING NORMAL AI SURFACE BUGS MAY STILL OCCUR)
	chrono::seconds timer(surfaceTime);
	AIPath[0][0] = AIPath[nextPath - 1][0]; AIPath[0][1] = AIPath[nextPath - 1][1]; // Sets last position to first position
	for (int i = 1; i <= nextPath + 2; i++) { // Clears all positions of the AI
		int x = 0; int y = 0;
		x = AIPath[i][1]; y = AIPath[i][0];
		AIPath[i][0] = 0; AIPath[i][1] = 0;
		AIMap[y][x] = false;
		AICompass[i] = "";
	}
	for (int i = 0; i < 6; i++) { // Clears all engineering
		north[i] = false; 	south[i] = false;
		east[i] = false; 	west[i] = false;
	}
	gunsBroke = false; specialBroke = false; detectionBroke = false;
	engNextPath = 1;
	nextPath = 1;
	shouldSurface = false;
	this_thread::sleep_for(timer);
	surfaceTime = 0;
	if (movementDone == true) { // checks if the user is doing something and if so sleeping 5 seconds longer
		chrono::seconds timer(5);
		this_thread::sleep_for(timer);
	}
	else {
		movementDone = false;
	}
}
bool easyCapUseitems(int use) { // A simple way to check if the AI can do anything. It was run this function between 2-3 times to see if it can do things
	use = rand() % 6;
	if (positionsSaved[0] == true) { use = rand() % 2 + 2; }
	if (use == 0 && mines.current == mines.max && !gunsBroke) {
		AICalled[0] = true;
		movementDone = true;
		mines.current = 0;
		return true;
	}
	else if (use == 1 && torpedo.current == torpedo.max && !gunsBroke) {
		if (AIPath[nextPath - 1][0] - triedPosition[userReadTries][0] <= 4
			&& AIPath[nextPath - 1][0] - triedPosition[userReadTries][0] >= -4
			|| AIPath[nextPath - 1][1] - triedPosition[userReadTries][1] <= 4
			&& AIPath[nextPath - 1][1] - triedPosition[userReadTries][1] >= -4
			&& torpedoCheck(AIPath[nextPath - 1][0], AIPath[nextPath - 1][1],
				triedPosition[userReadTries][0], triedPosition[userReadTries][1]) == true)
		{
			AICalled[1] = true;
			movementDone = true;
			torpedo.current = 0;
			return true;
		}
		else {
			use = 2; // If player is not close enough try to use drones
		}
	}
	else if (use == 2 && drones.current == drones.max && !detectionBroke) {
		AICalled[2] = true;
		movementDone = true;
		drones.current = 0;
		return true;
	}
	else if (use == 3 && sonar.current == sonar.max && !detectionBroke) {
		AICalled[3] = true;
		movementDone = true;
		sonar.current = 0;
		return true;
	}
	else if (use == 4 && silence.current == silence.max && !specialBroke) {
		AICalled[4] = true;
		movementDone = true;
		silence.current = 0;
		return true;
	}
	else if (use == 5 && scenario.current == scenario.max && !specialBroke) { // TODO Set up scenario
		AICalled[5] = true;
		movementDone = true;
		scenario.current = 0;
		return true;
	}
	else if (use == 6 && mineCount > 0 && !gunsBroke) {
		AICalled[6] = true;
		movementDone = true;
		return true;
	}
	return false;
}
bool normalCapUseitems(int use) { // Still being changed but a kinda more advanced way of checking
	use = rand() % 6;
	if (positionsSaved[0] == true) { use = rand() % 2 + 2; }
	if (use == 0 && mines.current == mines.max && !gunsBroke) {
		AICalled[0] = true;
		movementDone = true;
		mines.current = 0;
		return true;
	}
	else if (use == 1 && torpedo.current == torpedo.max && !gunsBroke) {
		if (radioOP.difficulty == 1){
			if (AIPath[nextPath - 1][0] - triedPosition[userReadTries][0] <= 4
				&& AIPath[nextPath - 1][0] - triedPosition[userReadTries][0] >= -4
				|| AIPath[nextPath - 1][1] - triedPosition[userReadTries][1] <= 4
				&& AIPath[nextPath - 1][1] - triedPosition[userReadTries][1] >= -4
				&& torpedoCheck(AIPath[nextPath - 1][0], AIPath[nextPath - 1][1],
					triedPosition[userReadTries][0], triedPosition[userReadTries][1]) == true)
			{
				AICalled[1] = true;
					movementDone = true;
					torpedo.current = 0;
					return true;
			}
			else {
				use = 2; // If player is not close enough try to use drones
			}
		}
		else {
			for (int i = 0; i < 2; i++) {
				if (AIPath[nextPath - 1][0] - userPossibleLocation[i][2] <= 4
					&& AIPath[nextPath - 1][0] - triedPosition[i][2] >= -4
					|| AIPath[nextPath - 1][1] - triedPosition[i][3] <= 4
					&& AIPath[nextPath - 1][1] - triedPosition[i][3] >= -4
					&& torpedoCheck(AIPath[nextPath - 1][0], AIPath[nextPath - 1][1],
						userPossibleLocation[i][2], userPossibleLocation[i][3]) == true)
				{
					AICalled[1] = true;
					movementDone = true;
					torpedo.current = 0;
					return true;
				}
			}
		}
		
	}
	else if (use == 2 && drones.current == drones.max && !detectionBroke) {
		AICalled[2] = true;
		movementDone = true;
		drones.current = 0;
		return true;
	}
	else if (use == 3 && sonar.current == sonar.max && !detectionBroke) {
		AICalled[3] = true;
		movementDone = true;
		sonar.current = 0;
		return true;
	}
	else if (use == 4 && silence.current == silence.max && !specialBroke) {
		AICalled[4] = true;
		movementDone = true;
		silence.current = 0;
		return true;
	}
	else if (use == 5 && scenario.current == scenario.max && !specialBroke) { // TODO Set up scenario
		AICalled[5] = true;
		movementDone = true;
		scenario.current = 0;
		return true;
	}
	else if (use == 6 && mineCount > 0 && !gunsBroke) { // TODO Change to userPossiblePositions for a more acurate hit
		for (int i = 0; i < 2; i++) {
			do {
				if (minesPosition[activeMine][0] - userPossibleLocation[i][2] <= 1
					&& minesPosition[activeMine][0] - triedPosition[i][2] >= -1
					|| minesPosition[activeMine][1] - triedPosition[i][3] <= 1
					&& minesPosition[activeMine][1] - triedPosition[i][3] >= -1)
				{
					AICalled[6] = true;
					movementDone = true;
					return true;
				}
			} while (activeMine < mineCount);
		}
	}
	return false;

}
void AIDecisions() {				// The captain, also know as decisions. Normal was just completed on 12/13/2019 8:43pm
	int currentPersonalityTime = 0; // Bugs may still be around for normal but all easy AI should work very well
	int x;
	int y;
	srand(time(NULL) - 100000000);
	bool positionDone = false;
	
	do {
		if (surfaceTime > 0) {
			doSurface();
		}
		while (!positionDone && !AIFirstPosition) {
			y = rand() % 15; AIPath[nextPath][0] = y;
			x = rand() % 15; AIPath[nextPath][1] = x;
			positionDone = noNoAreas(y, x);
			if (positionDone == true) {
				AIFirstPosition = true;
				nextPath++;
			}
		}
		if (captain.difficulty == 1) {
			while (!movementDone) {

				while (engineer.done == true && firstMate.done == true && !movementDone) {
					chrono::milliseconds dura1((rand() % 30 + 1 + captain.responseTime) * 100);
					this_thread::sleep_for(dura1);
					bool pause = false;
					int tries = rand() % 2 + 1;
					int use = 0;
					while (tries > 0) { // Triggers checking if it can use an item
						if (!easyCapUseitems(use)) {
							tries--;
						}
						else {break;}
					}
					if (!movementDone) { // checks randomly if it's gone really far and should surface
						if (nextPath >= rand() % 10 + 20) {
							AICalled[7] = true;	
							movementDone = true;
						}
						else if (shouldSurface == true && rand() % 5 == 0) { // If the engineer says to surface do so one fifth of the time (To keep the AI from surfacing all the time
							AICalled[7] = true;
							movementDone = true;
						}
						else { // This is where the Ai starts it's moves
							writePathDone = false; 
							writePath(y, x);
							if (AIPath[nextPath][0] == 16) { // If the write path gives error 16 then surface
								AICalled[7] = true;
								movementDone = true;
							}
							else { // If no error go as usual and move as told
								x = AIPath[nextPath][1]; 
								y = AIPath[nextPath][0];
								engineerUses++;
								firstMateUses++;
								engineer.done = false;
								firstMate.done = false;
								captain.done = true;
								nextPath++;
							}
						}
					}
				}
			}
		}
		else if (captain.difficulty == 2) {
			while (!movementDone) {
				while (engineer.done == true && firstMate.done == true && !movementDone) {
					chrono::milliseconds dura1((rand() % 30 + 1 + captain.responseTime) * 100);
					this_thread::sleep_for(dura1);
					if (currentPersonalityTime == 0) {
						int num = rand() % 3;
						currentPersonalityTime = rand() % 8 + 5;
						if (wants[1] == true || radioCertainty >= 3) { num = 2; }
						else if (wants[0] == true) { num = 3; }
						else if (wants[2] == true) { num = 1; }
						
						switch (num) {
						case 1: firstMateList[0] = 2; firstMateList[1] = 3; // Detect personality
							firstMateList[2] = 1; firstMateList[3] = 4;
							firstMateList[4] = 0; wants[1] = true; beInRangeofUser = 5;
							break;
						case 2: firstMateList[0] = 0; firstMateList[1] = 1; // Attack personality
							firstMateList[2] = 4; firstMateList[3] = 2;
							firstMateList[4] = 3; wants[0] = true; beInRangeofUser = 3;
							break;
						case 3: firstMateList[0] = 2; firstMateList[1] = 4; // Silence personality
							firstMateList[2] = 1; firstMateList[3] = 3;
							firstMateList[4] = 0; wants[2] = true; beInRangeofUser = 6;
							break;
						}
					}
					bool pause = false;
					int use = 0;
					int tries = rand() % 2 + 2;
					while (tries > 0) {
						if (!normalCapUseitems(use)) {
							tries--;
						}
						else { break; }

					}
					if (!movementDone) {
						if (nextPath >= rand() % 10 + 20) {
							AICalled[7] = true;
							movementDone = true;
						}
						else if (shouldSurface == true && rand() % 10 == 0) {
							AICalled[7] = true;
							movementDone = true;
						}
						else {
							writePathDone = false;
							writePath(y, x);
							if (AIPath[nextPath][0] == 16) {
								AICalled[7] = true;
								movementDone = true;
							}
							else {
								x = AIPath[nextPath][1];
								y = AIPath[nextPath][0];
								currentPersonalityTime--;
								engineerUses++;
								firstMateUses++;
								engineer.done = false;
								firstMate.done = false;
								captain.done = true;

								nextPath++;
							}
						}
					}
				}
			}

		}
		chrono::seconds set(1);
		this_thread::sleep_for(set);
	} while (!gameover);
}
void userMovement() { //This is run in the main thread checking if the user has entered to move and if so log it
	AICalledStop = true;
	if (_kbhit()) {
		switch (_getch())
		{
		case 'w':
			userPositions[userPositionCount] = "North";
			userPositionCount++;
			cout << "North" << endl;
			break;
		case 'd':
			userPositions[userPositionCount] = "East";
			userPositionCount++;
			cout << "East" << endl;
			break;
		case 's':
			userPositions[userPositionCount] = "South";
			userPositionCount++;
			cout << "South" << endl;
			break;
		case 'a':
			userPositions[userPositionCount] = "West";
			userPositionCount++;
			cout << "West" << endl;
			break;
		case KEY_ARROW_UP:
			userPositions[userPositionCount] = "North";
			userPositionCount++;
			cout << "North" << endl;
			break;
		case KEY_ARROW_DOWN:
			userPositions[userPositionCount] = "South";
			userPositionCount++;
			cout << "South" << endl;
			break;
		case KEY_ARROW_RIGHT:
			userPositions[userPositionCount] = "East";
			userPositionCount++;
			cout << "East" << endl;
			break;
		case KEY_ARROW_LEFT:
			userPositions[userPositionCount] = "West";
			userPositionCount++;
			cout << "West" << endl;
			break;
		case 'x': // Stops everything
			movementDone = true;
			AICalledStop = false;
			break;
		}
	}
}
void userSelection() { // If the user says to stop this is shown
	int y = 0;
	int x = 0;
	int num = rand() % 9;
	int count = 0;
	int input = 0;
	bool done = false;
	bool pop = false;
	cout << "\nYou have called to stop"
		<< "\n1. Surface"
		<< "\n2. Use Mines"
		<< "\n3. Use Torpedo"
		<< "\n4. Use Drones"
		<< "\n5. Use Sonar"
		<< "\n6. Use Silence"
		<< "\n7. Use Scenario"
		<< "\n8. AI moves"<< endl;
	do {
		cout << "Choice > ";
		cin >> input;
		if (input >= 1 && input <= 8) {
			switch (input){
			case 1: // User surface
				cout << "\nWhat sector are you in? > ";
				do {
					cin >> input;
					if (input >= 1 && input <= 9) {
						switch (radioOP.difficulty) {
						case 1:
							userSurfacePosition[2] = input;
							if (sectorPositionsCheck(triedPosition[userReadTries][0], triedPosition[userReadTries][1], input) == true) {
								userSurfacePosition[0] = triedPosition[userReadTries][0];
								userSurfacePosition[1] = triedPosition[userReadTries][1];
							}
							else {
								for (int i = 0; i <= userReadTries; i++) { // Clears all positions from where the AI thought the player was
									int x = 0; int y = 0;
									x = triedPosition[i][1]; y = triedPosition[i][0];
									triedPosition[i][0] = 0; triedPosition[i][1] = 0;
									userDeadZones[y][x] = false;
								}
								userReadTries = 0;
							}
							pop = true;
							break;
						case 2:
							for (int x = 0; x < 4; x++) {
								userCompassCounts[x] = 0;
							}
							for (int i = 0; i < 9; i++) {
								userSectorStarters[i] = true;
								if (i == input - 1) {
									userSectorStarters[i] = false;
									sectorTrying = 0;
								}
							}
							for (int i = 0; i < 3; i++) {
								if (sectorPositionsCheck(userPossibleLocation[i][0],userPossibleLocation[i][1], input) == true 
									|| sectorPositionsCheck(userPossibleLocation[i][1], userPossibleLocation[i][1], input) == true) {
									
								}
								else {
									userPossibleLocation[i][0] = 0; userPossibleLocation[i][0] = 0;
								}
							}
							pop = true;
							break;
						}
					}
					else {
						cout << "\nEnter a number between 1-9 > ";
					}
				} while (!pop);
				break;
			case 2: // user place mine
				cout << "\nMark the mine on your map" << endl;
				system("pause");
				break;
			case 3: // user shoot torpedo
				cout << "\nEnter the Y cord > ";
				cin >> y;
				cout << "\nEnter the X cord > ";
				cin >> x;
				if (AIPath[nextPath - 1][0] == y && AIPath[nextPath - 1][1] == x) {
					cout << "\nDirect hit! AI takes 2 points of damamge" << endl;
					AIHealth -= 2;
				}
				else if (AIPath[nextPath - 1][0] == y - 1 && AIPath[nextPath - 1][1] == x - 1) {
					cout << "\nIndirect hit! AI takes 1 points of damamge" << endl;
					AIHealth -= 1;
				}
				else if (AIPath[nextPath - 1][0] == y - 1 && AIPath[nextPath - 1][1] == x) {
					cout << "\nIndirect hit! AI takes 1 points of damamge" << endl;
					AIHealth -= 1;
				}
				else if (AIPath[nextPath - 1][0] == y - 1 && AIPath[nextPath - 1][1] == x + 1) {
					cout << "\nIndirect hit! AI takes 1 points of damamge" << endl;
					AIHealth -= 1;
				}
				else if (AIPath[nextPath - 1][0] == y && AIPath[nextPath - 1][1] == x - 1) {
					cout << "\nIndirect hit! AI takes 1 points of damamge" << endl;
					AIHealth -= 1;
				}
				else if (AIPath[nextPath - 1][0] == y && AIPath[nextPath - 1][1] == x + 1) {
					cout << "\nIndirect hit! AI takes 1 points of damamge" << endl;
					AIHealth -= 1;
				}
				else if (AIPath[nextPath - 1][0] == y + 1 && AIPath[nextPath - 1][1] == x - 1) {
					cout << "\nIndirect hit! AI takes 1 points of damamge" << endl;
					AIHealth -= 1;
				}
				else if (AIPath[nextPath - 1][0] == y + 1 && AIPath[nextPath - 1][1] == x) {
					cout << "\nIndirect hit! AI takes 1 points of damamge" << endl;
					AIHealth -= 1;
				}
				else if (AIPath[nextPath - 1][0] == y + 1 && AIPath[nextPath - 1][1] == x + 1) {
					cout << "\nIndirect hit! AI takes 1 points of damamge" << endl;
					AIHealth -= 1;
				}
				else {
					cout << "\nThat was a miss! Thanks for giving me more info about your location" << endl;
				}
				userTorpedoPosition[0] = y; userTorpedoPosition[1] = x;
				switch (radioOP.difficulty) {
				case 1:
					if (triedPosition[userReadTries][0] - userTorpedoPosition[0] <= 4
						&& triedPosition[userReadTries][0] - triedPosition[userReadTries][0] >= -4
						|| triedPosition[userReadTries][1] - userTorpedoPosition[1] <= 4
						&& triedPosition[userReadTries][1] - userTorpedoPosition[1] >= -4) {
						keepUserPosition = true;
					}
					else {
						keepUserPosition = false;
						userReadTries = 0;
					}
					break;
				case 2:
					for (int i = 0; i < 2; i++) {
						if (userPossibleLocation[i][2] - userTorpedoPosition[0] <= 4
							&& userPossibleLocation[i][2] - triedPosition[userReadTries][0] >= -4
							|| userPossibleLocation[i][3] - userTorpedoPosition[1] <= 4
							&& userPossibleLocation[i][3] - userTorpedoPosition[1] >= -4) {
							radioCertainty++;
						}
						else {
							userPossibleLocation[i][0] = 0; userPossibleLocation[i][1] = 0; positionsSaved[i] = false;
							userPossibleLocation[i][2] = 0; userPossibleLocation[i][3] = 0; radioCertainty--;
						}
					}
					break;
				}
				system("pause");
				break;
			case 4: // user drone
				cout << "\nWhat sector do you think I'm in > ";
				cin >> input;
				if (sectorPositionsCheck(AIPath[nextPath - 1][0], AIPath[nextPath - 1][1], input) == true) {
					cout << "\nI am in sector " << input << " come and get me!" << endl;
				}
				else {
					cout << "\nI am not in sector " << input << endl;
				}
				system("pause");
				break;
			case 5: // user sonar
				num = rand() % 6;
				if (num == 0) { cout << "\nI'm in Y > " << AIPath[nextPath - 1][0] + 1 << ", X > " << rand() % 15 + 1 << endl; }
				else if (num == 1) { cout << "\nI'm in Y > " << AIPath[nextPath - 1][0] + 1 << ", Sector " << rand() % 9 + 1 << endl; }
				else if (num == 2) { cout << "\nI'm in Y > " << rand() % 15 + 1 << ", X > " << AIPath[nextPath - 1][1] + 1 << endl; }
				else if (num == 3) { cout << "\nI'm in Y > " << rand() % 15 + 1 << ", Sector " << sectorPositions(AIPath[nextPath - 1][0], AIPath[nextPath - 1][1]) << endl; }
				else if (num == 4) { cout << "\nI'm in X > " << AIPath[nextPath - 1][1] + 1<< ", Sector " << rand() % 9 + 1 << endl; }
				else if (num == 5) { cout << "\nI'm in X > " << rand() % 15 + 1 << ", Sector " << sectorPositions(AIPath[nextPath - 1][0],AIPath[nextPath - 1][1]) << endl; }
				system("pause");
				break;
			case 6: // user silence 
				cout << "\nPress any key when you are done" << endl;
				system("pause");
				switch (radioOP.difficulty) {
				case 1:
					for (int i = 0; i <= userReadTries; i++) { // Clears all positions from where the AI thought the player was
						int x = 0; int y = 0;
						x = triedPosition[i][1]; y = triedPosition[i][0];
						triedPosition[i][0] = 0; triedPosition[i][1] = 0;
						userDeadZones[y][x] = false;
					}
					userReadTries = 0;
					keepUserPosition = false;
					break;
				case 2:
					silenceReadTries = userReadTries; // TODO Figure out math for how to find user after silence is used
					radioCertainty = 0;
					break;
				}
				break;
			case 7:

				break;
			case 8: // Shows all user moves
				cout << "\nMy moves were" << endl;
				for (int i = 1; i < nextPath; i++) {
					cout << AICompass[i] << endl;
				}
				system("pause");
				break;
			case 9:
				switch (radioOP.difficulty) {
				case 1:
					cout << "\nUndo" << endl;
					triedPosition[userReadTries][0] = 0; triedPosition[userReadTries][1] = 0;
					userPositions[userReadTries] = "";
					userReadTries--;
					break;
				case 2:
					userPositions[userReadTries] = "";
					userReadTries--;

					break;
				}
				break;
			}
			done = true;
		}
		else {
			cout << "\nPlease enter a proper number ";
		}
	} while (!done);
}
int AISelection() { // If the AI says to stop
	bool check = false;
	bool positionDone = false;
	int input2;
	char input;
	system("pause");
	if (AICalled[0] == true) { //ai wants to place a mine
		int mineX = 0;
		int mineY = 0;
		do {
			mineY = AIPath[nextPath - 1][0] + rand() % 3 - 1;
			mineX = AIPath[nextPath - 1][1] + rand() % 3 - 1;
			positionDone = noNoAreas(mineY, mineX);
		} while (!positionDone);
		cout << "AI Has placed a mine!" << endl;
		minesPosition[mineCount][0] = mineY; minesPosition[mineCount][1] = mineX;
		AIMap[mineY][mineX] = true;
		mineCount++;
		system("pause");
		AICalled[0] = false;
	}
	else if (AICalled[1] == true) { // Ai wants to shoot a big boy mmm
		cout << "\nAI Is shooting a torpedo!" << endl;
		chrono::seconds dura1(2);
		this_thread::sleep_for(dura1);
		cout << "\nThe torpedo hits at Y > " << triedPosition[userReadTries][0] << " X > " << triedPosition[userReadTries][1] << endl;
		cout << "\nDoes that hit? Y/N > ";

		do {
			cin >> input;
			if (input == 'N' || input == 'n') {
				keepUserPosition = false;
				check = true;
			}
			else if (input == 'Y' || input == 'y') {
				keepUserPosition = true;
				check = true;
			}
			else {
				cout << "\nEnter correct leters > ";
			}
		} while (!check);
		AICalled[1] = false;
	}
	else if (AICalled[2] == true) { // AI wants to launch a drone
		int num = 0;
		switch (radioOP.difficulty) {
		case 1:
			num = sectorPositions(triedPosition[userReadTries - 1][0], triedPosition[userReadTries - 1][1]);
			break;
		case 2: 
			num = sectorPositions(userDroneCheckPoss[0], userDroneCheckPoss[1]);
			break;
		}
		
		cout << "\nAI Is lanching a drone!" << endl;
		chrono::seconds dura1(2);
		this_thread::sleep_for(dura1);
		cout << "\nAre you in Sector " << num << "? Y/N > ";
		
		if (YNInputs() == 'n') {
			keepUserPosition = false;
			userPossibleLocation[userDroneCheckPoss[2]][0] = 0; userPossibleLocation[userDroneCheckPoss[2]][1] = 0; 
			userPossibleLocation[userDroneCheckPoss[2]][2] = 0; userPossibleLocation[userDroneCheckPoss[2]][3] = 0;
			positionsSaved[userDroneCheckPoss[2]] = false;
		}
		else {
			keepUserPosition = true;
		}
		
		AICalled[2] = false;
	}
	else if (AICalled[3] == true) { // AI wants to scan 
		int num = 1;
		int times = 0;
		bool one = false; bool two = false; bool three = false;
		cout << "\nAI Is activating sonar!" << endl;
		chrono::seconds dura1(2);
		this_thread::sleep_for(dura1);
		do {
			if (!one) {
				cout << num << ". X cord\n"; num++;
			}
			if (!two) {
				cout << num << ". Y cord\n"; num++;
			}
			if (!three) {
				cout << num << ". Sector\n";
			}
			cin >> input2;
			if (input2 == 1 && !one) {
				cout << "X position 1-15 > ";
				cin >> userSonarHints[1]; userSonarHints[1]--;
				one = true; times++;
			}
			else if (input2 == 2 && one == true || input2 == 1) {
				cout << "Y position 1-15 > ";
				cin >> userSonarHints[0]; userSonarHints[0]--;
				two = true; times++;
			}
			else if (input2 == num) {
				cout << "Sector 1-9 > ";
				cin >> userSonarHints[2]; 
				three = true; times++;
			}
			else {
				cout << "\nEnter correct numbers";
			}
			num = 1;
			if (times >= 2) {
				check = true;
			}
		} while (!check);
		switch (radioOP.difficulty) {
		case 1:

			break;
		case 2:
			userSonarHints[3] = userReadTries;
			break;
		}
		AICalled[3] = false;
	}
	else if (AICalled[4] == true) {
		int count = 0;
		int z = 0;
		int moreLess = 0;
		int type = rand() % 2; // TODO See if silence works
		if (type == 0) {
			z = AIPath[nextPath - 1][0];
		}
		else {
			z = AIPath[nextPath - 1][1];
		}
		cout << "\nAI Is activating silence!" << endl;
		chrono::seconds dura1(3);
		this_thread::sleep_for(dura1);
		do {
			do {
			if (rand() % 2 == 0) {
				z--; moreLess = 1;
			}
			else {
				z++; moreLess = -1;
			}
			if (type == 0) {
				positionDone = noNoAreas(z, AIPath[nextPath - 1][1]);
			}
			else {
				positionDone = noNoAreas(AIPath[nextPath - 1][0], z);
			}

			} while (!positionDone);
			do {
				count++;
				AIPath[nextPath][type] = z;
				z += moreLess;
				if (type == 0) {
					positionDone = noNoAreas(z, AIPath[nextPath - 1][1]);
				}
				else {
					positionDone = noNoAreas(AIPath[nextPath - 1][0], z);
				}

			} while (positionDone == true && count <= 4);
			if (count <= 1) {
				z -= moreLess;
			}
			else {
				check = true;
			}
		} while (!check);
		cout << "\nAI is ready" << endl;
		AICalled[4] = false;
		system("pause");

	}
	else if (AICalled[5] == true) { // Scenario
	AICalled[5] = false;
	}
	else if (AICalled[6] == true) { // Activate a mine
	int num = 0;
	if (captain.difficulty == 1){
		num = rand() % mineCount;
	}
	else {
		num = activeMine;
	}

		cout << "\nAI is activating a mine" << endl;
		chrono::seconds dura1(2);
		this_thread::sleep_for(dura1);
		cout << "\nMine detonated at Y > " << minesPosition[num][0] << " X > " << minesPosition[num][1] << endl;
		minesPosition[mineCount - 1][0] = 0; minesPosition[mineCount - 1][1] = 0;
		cout << "\nDoes that hit? Y/N > ";
		cin >> input2;
		mineCount--;
		AICalled[6] = false;
}
	else if (AICalled[7] == true) {
		int num = rand() % 20 + 20;
		cout << "\nAI is in Sector " << sectorPositions(AIPath[nextPath - 1][0], AIPath[nextPath - 1][1]) << endl;
		cout << "\nPlay will continue while AI fixes Engineering. Est > " << num << " seconds" << endl;
		system("pause");
		AISurfaced = true;
		AICalled[7] = false;
		return num;
	}
	return 0;
}
void start() { // Starts the game
	int position;
	int x;
	int input2 = 1;
	char input;
	bool done = false;
	string mapInput;
	system("cls");
	do {
		if (!mapChosen) { // If user didn't select a map in the map menu then ask for a map to be entered
			cout << "Enter a * or - 15 times with spaces inbetween" << endl;
			cout << "   A B C D E F G H I J K L M N O" << endl;
			do {
				position = 0;
				cout << input2 << ". ";
				while (position < 15) {
					x = position;
					position++;
					cin >> mapInput;
					if (mapInput == "-") {
						mapBoundries[input2 - 1][x] = true;
						AIMap[input2 - 1][x] = true;
						userDeadZones[input2 - 1][x] = true;
						AIRadipOPTriedSpaces[input2 - 1][x] = true;
					}
				}
				input2++;
			} while (input2 < 16);
			done = true;
		}
		else { // Ask if the user is sure that they want the map and if yes contiune
			cout << "It seems you already have a map picked out. Keep map? Y/N > ";
			if (YNInputs() == 'n') { 
				done = false;
				x = 0;
				for (int i = 0; i < 15; i++) {
					mapBoundries[x][i] = false;
					AIMap[x][i] = false;
					userDeadZones[x][i] = false;
					AIRadipOPTriedSpaces[x][i] = false;
					if (i >= 14) {
						x++; i = -1;
					}if (x >= 15) {
						break;
					}
				}
				mapChosen = false;
			}
			else { done = true; }
		}
	} while (!done);
	system("pause");
}
string setDifficulty(string name) { // Sets the difficulty of all the AI
	int input2 = 0;
	system("cls");
	cout << "     Set the Difficulty of all AI     "
		<< "\n1. Easy (Makes the AI random)"
		<< "\n2. Normal (The AI makes slightly random)"
		<< "\n3. Hard (This AI will work out what it needs to do)" << endl;
	while (!menuDone) {
		cin >> input2;
		if (input2 == 1) {
			captain.responseTime = 20;
			radioOP.responseTime = 20;
			engineer.responseTime = 20;
			firstMate.responseTime = 20;
			captain.type = "Easy";
			radioOP.type = "Easy";
			engineer.type = "Easy";
			firstMate.type = "Easy";
			name = "Easy";
			break;
		}
		else if (input2 == 2) {
			captain.responseTime = 10;
			radioOP.responseTime = 10;
			engineer.responseTime = 10;
			firstMate.responseTime = 10;
			captain.type = "Normal";
			radioOP.type = "Normal";
			engineer.type = "Normal";
			firstMate.type = "Normal";
			name = "Normal";
			break;
		}
		else if (input2 == 3) {
			captain.responseTime = 0;
			radioOP.responseTime = 0;
			engineer.responseTime = 0;
			firstMate.responseTime = 0;
			captain.type = "Hard";
			radioOP.type = "Hard";
			engineer.type = "Hard";
			firstMate.type = "Hard";
			name = "Hard";
			break;
		}
		else {
			cout << "Please enter valid numbers ";
		}
	}
	captain.difficulty = input2;
	radioOP.difficulty = input2;
	engineer.difficulty = input2;
	firstMate.difficulty = input2;
	system("cls");
	return name;
}
bool changed[4] = { false, false, false, false };
string setCustom(string name) { // Starts the big process of allowing a user to change any detail they want about the game
	int input = 0;
	int input2 = 0;
	bool done = false;
	bool done2 = false;
	system("cls");
	do {
		cout << "     Set the game parameters of the game     "
			<< "\n1. Setup First Mate Gauges"
			<< "\n2. Setup AI Radio Difficulty"
			<< "\n3. Setup AI Captian Difficulty"
			<< "\n4. Setup AI First Mate Difficulty"
			<< "\n5. Setup AI Engineer Difficulty"
			<< "\n6. Exit" << endl;
		cout << "\nChoice > ";
		cin >> input;
		if (input >= 1 && input <= 5) {
			system("cls");
			switch (input) {
			case 1:
				do {
					cout << "     Set custom Gauges     "
						<< "\n1. Set Mines max (Currently " << mines.max << " )"
						<< "\n2. Set Torpedo max (Currently " << torpedo.max << " )"
						<< "\n3. Set Sonar max (Currently " << sonar.max << " )"
						<< "\n4. Set Drones max (Currently " << drones.max << " )"
						<< "\n5. Set Scenario max (Currently " << scenario.max << " )"
						<< "\n6. Set All maxes to normal"
						<< "\n7. Exit" << endl;
					cout << "\nChoice > ";
					cin >> input;
					if (input >= 1 && input <= 6) {
						cout << "Set max to what? > ";
						cin >> input2;
						switch (input) {
						case 1:
							mines.max = input2;
							break;
						case 2:
							torpedo.max = input2;
							break;
						case 3:
							sonar.max = input2;
							break;
						case 4:
							drones.max = input2;
							break;
						case 5:
							scenario.max = input2;
							break;
						case 6:
							mines.max = 3;
							torpedo.max = 3;
							sonar.max = 3;
							drones.max = 5;
							silence.max = 6;
							scenario.max = 8;
							break;
						}

						system("cls");
					}
					else if (input == 7) {
						done2 = true;
						system("cls");
					}
					else {
						cout << "\nPlease enter the right numbers" << endl;
					}
				} while (!done2);
				break;
			case 2:
				do {
					cout << "     Set AI Radio Difficulty      "
						<< "\n1. Set Difficulty (Current difficulty " << radioOP.type << " )"
						<< "\n2. Set Response Time in seconds (Current response time " << radioOP.responseTime / 10 << " )"
						<< "\n3. Exit" << endl;
					cout << "\nChoice > ";
					cin >> input;
					if (input == 1) {
						system("cls");
						cout << "\n1. Easy (Makes the AI random)"
							<< "\n2. Normal (The AI makes slightly random)"
							<< "\n3. Hard (This AI will work out what it needs to do)" << endl;
						cin >> input;
						if (input >= 1 && input <= 3) {
							radioOP.difficulty = input;
							switch (input) {
							case 1: radioOP.type = "Easy"; if (!changed[0]) { radioOP.responseTime = 20; } break;
							case 2: radioOP.type = "Normal"; if (!changed[0]) { radioOP.responseTime = 10; } break;
							case 3: radioOP.type = "Hard"; if (!changed[0]) { radioOP.responseTime = 0; } break;
							}
							name = "Custom";
						}
						system("cls");
					}
					else if (input == 2) {
						cout << "\nHow many seconds should delay this AI? > ";
						cin >> input;
						radioOP.responseTime = input * 10;
						changed[0] = true;
						system("cls");
					}
					else if (input == 3) {
						done2 = true;
						system("cls");
					}
					else {
						cout << "\nPlease enter the right numbers" << endl;
					}
				} while (!done2);
					break;
			case 3:
				do {
					cout << "     Set AI Captain Difficulty      "
						<< "\n1. Set Difficulty (Current difficulty " << captain.type << " )"
						<< "\n2. Set Response Time in seconds (Current response time " << captain.responseTime / 10 << " )"
						<< "\n3. Exit" << endl;
					cout << "\nChoice > ";
					cin >> input;
					if (input == 1) {
						system("cls");
						cout << "\n1. Easy (Makes the AI random)"
							<< "\n2. Normal (The AI makes slightly random)"
							<< "\n3. Hard (This AI will work out what it needs to do)" << endl;
						cin >> input;
						if (input >= 1 && input <= 3) {
							captain.difficulty = input;
							switch (input) {
							case 1: captain.type = "Easy"; if (!changed[0]) { captain.responseTime = 20; } break;
							case 2: captain.type = "Normal"; if (!changed[0]) { captain.responseTime = 10; } break;
							case 3: captain.type = "Hard"; if (!changed[0]) { captain.responseTime = 0; } break;
							}
							name = "Custom";
							system("cls");
						}
					}
					else if (input == 2) {
						cout << "\nHow many seconds should delay this AI? > ";
						cin >> input;
						captain.responseTime = input * 10;
						changed[0] = true;
						system("cls");
					}
					else if (input == 3) {
						done2 = true;
						system("cls");
					}
					else {
						cout << "\nPlease enter the right numbers" << endl;
					}
				} while (!done2);
				break;
			case 4:
				do {
					cout << "     Set AI First Mate Difficulty      "
						<< "\n1. Set Difficulty (Current difficulty " << firstMate.type << " )"
						<< "\n2. Set Response Time in seconds (Current response time " << firstMate.responseTime / 10 << " )"
						<< "\n3. Exit" << endl;
					cout << "\nChoice > ";
					cin >> input;
					if (input == 1) {
						system("cls");
						cout << "\n1. Easy (Makes the AI random)"
							<< "\n2. Normal (The AI makes slightly random)"
							<< "\n3. Hard (This AI will work out what it needs to do)" << endl;
						cin >> input;
						if (input >= 1 && input <= 3) {
							firstMate.difficulty = input;
							switch (input) {
							case 1: firstMate.type = "Easy"; if (!changed[0]) { firstMate.responseTime = 20; } break;
							case 2: firstMate.type = "Normal"; if (!changed[0]) { firstMate.responseTime = 10; } break;
							case 3: firstMate.type = "Hard"; if (!changed[0]) { firstMate.responseTime = 0; } break;
							}
							name = "Custom";
							system("cls");
						}
					}
					else if (input == 2) {
						cout << "\nHow many seconds should delay this AI? > ";
						cin >> input;
						firstMate.responseTime = input * 10;
						changed[0] = true;
						system("cls");
					}
					else if (input == 3) {
						done2 = true;
						system("cls");
					}
					else {
						cout << "\nPlease enter the right numbers" << endl;
					}
				} while (!done2);
				break;
			case 5:
				do {
					cout << "     Set AI Engineer Difficulty      "
						<< "\n1. Set Difficulty (Current difficulty " << engineer.type << " )"
						<< "\n2. Set Response Time in seconds (Current response time " << engineer.responseTime / 10 << " )"
						<< "\n3. Exit" << endl;
					cout << "\nChoice > ";
					cin >> input;
					if (input == 1) {
						system("cls");
						cout << "\n1. Easy (Makes the AI random)"
							<< "\n2. Normal (The AI makes slightly random)"
							<< "\n3. Hard (This AI will work out what it needs to do)" << endl;
						cin >> input;
						if (input >= 1 && input <= 3) {
							engineer.difficulty = input;
							switch (input) {
							case 1: engineer.type = "Easy"; if (!changed[0]) { engineer.responseTime = 20; } break;
							case 2: engineer.type = "Normal"; if (!changed[0]) { engineer.responseTime = 10; } break;
							case 3: engineer.type = "Hard"; if (!changed[0]) { engineer.responseTime = 0; } break;
							}
							name = "Custom";
							system("cls");
						}
					}
					else if (input == 2) {
						cout << "\nHow many seconds should delay this AI? > ";
						cin >> input;
						engineer.responseTime = input * 10;
						changed[0] = true;
						system("cls");
					}
					else if (input == 3) {
						done2 = true;
						system("cls");
					}
					else {
						cout << "\nPlease enter the right numbers" << endl;
					}
				} while (!done2);
				break;
			}
		}
		else if (input == 6) {
			done = true;
		}
		else {
			system("cls");
			cout << "\nPlease enter the right numbers" << endl;
		}
	} while (!done);
	system("cls");
	return name;
}
void gameDATAStorage() { // Opens the data file that for now only houses how many maps there are
	ofstream dataFile; // Opens writing to the file
	dataFile.open("GameData.txt");
	dataFile << mapNumber << " ";
	dataFile.close();
}
void mapTypes() { // Starts the map setup process
	int position;
	int x = 0;
	int times = 0;
	int input2 = 1;
	string mapInput;
	bool givenInput = false;
	bool done = false;
	int input = 0;
	bool check = false;
	char StrInput;
	char mapDATA[15][15];
	system("cls");
	do {
		check = false;
		cout << "     Set Maps     "
			<< "\n1. Add a new map"
			<< "\n2. See existing maps or set map for battle"
			<< "\n3. New file (NOT READY YET)"
			<< "\n4. Exit" << endl;
		cin >> input;
		if (input == 1) {
			ofstream inputFile; // Opens writing to the file
			inputFile.open("Maps.txt", ios::app);
			system("cls");
			cout << "Are you sure you want to add a map? Y/N > ";
			do {
				cin >> StrInput;
				if (StrInput == 'N' || StrInput == 'n') {
					check = true;
				}
				else if (StrInput == 'Y' || StrInput == 'y') {
					system("cls");
					mapNumber++;
					gameDATAStorage();
					cout << "Enter a * or - 15 times with spaces inbetween" << endl;
					cout << "   A B C D E F G H I J K L M N O" << endl;
					do {
						position = 0;
						cout << input2 << ". ";
						while (position < 15) {
							x = position;
							position++;
							cin >> mapInput;
							inputFile << mapInput << " ";
						}
						input2++;
						inputFile << "\n";
					} while (input2 < 16);
					inputFile << "\n";
					input2 = 1;

					check = true;
					system("cls");
				}
				else {
					cout << "\nEnter correct leters > ";
				}
			} while (!check);
			inputFile.close();
		}
		else if (input == 2) {
			bool LEFTRIGHT = false;
			int num = 0;
			do {
				ifstream currentMap; // Opens reading to the file
				currentMap.open("Maps.txt");
				times = 0;
				do {
					x = 0;
					for (int i = 0; i < 15; i++) {
						currentMap >> mapDATA[x][i];
						if (mapDATA[0][0] != '*' && mapDATA[0][0] != '-') { cout << "No new maps";  break; }
						if (i >= 14) {
							x++;
							i = -1;
						}
						if (x >= 15) {
							break;
						}
					}
					times++;
				} while (times <= num);

				system("cls");
				cout << "           Map " << num + 1 << endl;
				if (times <= num) { times = 0; }
				do {
					x = 0;
					for (int i = 0; i < 15; i++) {
						cout << mapDATA[x][i] << " ";
						if (i >= 14) {
							x++;
							cout << "\n";
							i = -1;
						}
						if (x >= 15) {
							break;
						}
					}
					times++;
				} while (times <= num);
				cout << "\nUse arrow keys or A and W to navigate maps that are saved";
				cout << "\nPress x anytime to exit this menu"
					<< "\nPress s to save map to battle plan"
					<< "\nPress c to clear saved map";
				if (num < 0 || num >= mapNumber) { if (!LEFTRIGHT) { num++; } else { num--; } cout << "\nNO MORE MAPS"; givenInput = true; }
				while (!givenInput) {
					if (_kbhit()) {
						switch (_getch())
						{
						case 'd':
							num++;
							LEFTRIGHT = true;
							givenInput = true;
							break;
						case 'a':
							num--;
							LEFTRIGHT = false;
							givenInput = true;
							break;
						case KEY_ARROW_RIGHT:
							num++;
							LEFTRIGHT = true;
							givenInput = true;
							break;
						case KEY_ARROW_LEFT:
							num--;
							LEFTRIGHT = false;
							givenInput = true;
							break;
						case 'c':
							x = 0;
							for (int i = 0; i < 15; i++) {
								mapBoundries[x][i] = false;
								AIMap[x][i] = false;
								userDeadZones[x][i] = false;
								AIRadipOPTriedSpaces[x][i] = false;
								if (i >= 14) {
									x++; i = -1;
								}if (x >= 15) {
									break;
								}
							}
								mapChosen = false;
								break;
						case 's':
							times = 0;
							x = 0;
							input2 = 0;
							position = 0;
							do {
								for (int i = 0; i < 15; i++) {
									mapBoundries[x][i] = false;
									AIMap[x][i] = false;
									userDeadZones[x][i] = false;
									AIRadipOPTriedSpaces[x][i] = false;
									if (i >= 14) {
										x++; i = -1;
									}if (x >= 15) {
										break;
									}
								}
								x = 0;
								while (position < 15) {
									x = position;
									position++;
									do {
										x = 0;
										for (int i = 0; i < 15; i++) {
											currentMap >> mapInput;
											if (times == num) {
												if (mapInput == "-") {
													mapBoundries[input2][x] = true;
													AIMap[input2][x] = true;
													userDeadZones[input2][x] = true;
													AIRadipOPTriedSpaces[input2][x] = true;
												}
											}
											if (i >= 14) {
												x++;
												i = -1;
											}
											if (x >= 15) {
												break;
											}
										}
										times++;
									} while (times <= num);

								}
								input2++;
							} while (input2 < 16);
							cout << "\nMap saved";
							mapChosen = true;
							break;
						case 'x':
							givenInput = true;
							check = true;
							break;
							}
						}
					}
					currentMap.close();
					givenInput = false;
				} while (!check);
				system("cls");
			
		}
		else if (input == 4) {
			done = true;
		}
		else {
			cout << "Please enter valid numbers ";
		}
	} while (!done);
	
	
	system("cls");
}
void info() {
	cout << "            Info      " << endl; // TODO Set up info
	cout << "\nHow to play:"
		<< "\nChoose a map in the menu or create your own map. This can also be done after entering start"
		<< "\nOpen space = *"
		<< "\nClosed space = -"
		<< "\n\nTo interact with the AI use arrow keys or WASD for north, south, west, east"
		<< "\nTo stop the AI for surfacing or anything else press x" 
		<< "\n\nIn the customize menu you are able to change everything about the game."
		<< "\nHow fast each AI reacts or each AI difficulty" << endl;
	system("pause");
	system("cls");
}
void takeDATAStorage() {
	ifstream data; // Opens reading to the file
	data.open("GameData.txt");
	data >> mapNumber;
	data.close();
}
int main() {
	takeDATAStorage();
	mines.max = 3;
	torpedo.max = 3;
	sonar.max = 3;
	drones.max = 4;
	silence.max = 6;
	scenario.max = 6;
	engineer.done = false;
	firstMate.done = false;
	string AIOverallDiff = "Easy";
	char input;
	srand(time(NULL));
		while (!menuDone) {
			cout << "Welcome to the Captain Sonar AI program"
				<< "\n\nPress s to start"
				<< "\n\nPress d to set difficulty (Current difficulty " << AIOverallDiff << ")"
				<< "\n\nPress i for info"
				<< "\n\nPress c to customize game"
				<< "\n\nPress m to set map"
				<< "\n\nPress x to exit the program\n\n";
			cout << "Choice > ";
			cin >> input;
			if (input == 's' || input == 'S') {
				int num = 1;
				start();
				gameover = false;
				movementDone = true;
				thread T4(&AIRadioOP); // Opens up all of the threads
				thread T1(&AIDecisions);
				thread T3(&AIEngineer);
				thread T2(&AIFirstMate);
				while (!gameover) {
					for (int i = 3; i > 0; i--) {
						system("cls");
						cout << "Starting in " << i;
						chrono::seconds dura1(1);
						this_thread::sleep_for(dura1);
					}
					system("cls");
					movementDone = false;
					
					while (!movementDone) { // TODO Add First Mate Cout to main thread
						userMovement();
						if (AICompass[num] != "" && writePathDone == true) { cout << "AI moves " << AICompass[num] << endl;
						num++; }
					}
					if (!AICalledStop) {
						userSelection();
					}
					if (AICalledStop == true) {
						if (AICalled[7] == true) { cout << "AI surfaces!!\a" << endl; num = 1; }
						cout << "AI Calls to stop!\a" << endl;
						surfaceTime = AISelection();
					}
					if (AIHealth <= 0) { // If AI health is 0 end game
						gameover = true;
						T1.join();
						T2.join();
						T3.join();
						T4.join();
						cout << "\nYou have won!! Goodjob!\a" << endl;
						system("pause");
					}
				}
			}
			else if (input == 'd' || input == 'D') {
				AIOverallDiff = setDifficulty(AIOverallDiff);
			}
			else if (input == 'i' || input == 'I') {
				info();
			}
			else if (input == 'c' || input == 'C') {
				AIOverallDiff = setCustom(AIOverallDiff);
			}
			else if (input == 'm' || input == 'M') {
				mapTypes();
			}
			else if (input == 'x' || input == 'X') {
				menuDone = true;
				break;
			}
			else {
				cout << "Please enter correct letters\a\n" << endl;
			}
		}
	return 0;
}