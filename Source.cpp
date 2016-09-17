#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>
using namespace std;

#pragma region STRUCTURES

struct Board
{
	int squares[9];
};

struct Move
{
	int position = 0;
	bool works = true;
};

#pragma endregion

#pragma region GLOBAL VARIABLES

int victor = 0; //1 = player wins, -1 = computer wins
int numBoards;
bool realRun = true;
bool giveTestCaseOption = true; //Makes sure that the test cases aren't run twice in a row
const int numTrials = 2000; //10000 was the old default, but it took too long
bool clearFilesFirst = false;

#pragma endregion

#pragma region FUNCTION PROTOTYPES

//normal functions
void display(const Board);
char getDisplayChar(const int &);
bool playerMove(Board &); //returns true if win
bool computerMove(Board &, int &, int &);
bool victory(const Board &, int);
bool tie(const Board &);
bool isMovePossible(const Board &);

bool operator== (const Board &, const Board &);

//functions that control AI
Move findNextMove(const Board &);
Move findNextMove(const Board &, const Move &);
void executeMove(Board &, const Move &, const int & = -1);
int findCurrentBoardLocationInFile(const Board &);
int countNumBoards();

#pragma endregion

int main()
{
	ofstream f1("moves.txt", ios::out | ios::app);
	ofstream f2("boards.txt", ios::out | ios::app);
	f1.close();
	f1.close();
	ifstream tempFile("moves.txt", ios::ate | ios::binary);
	bool emptyFile = ((long)(tempFile.tellg()) == 0);
	tempFile.close();

	char reply;

	if (giveTestCaseOption)
	{
		if (!emptyFile)
		{
			do
			{
				cout << "Would you like to clear the test case file to begin the AI performance \nfrom scratch? (Y/N): ";
				cin >> reply;
			} while (toupper(reply) != 'Y' && toupper(reply) != 'N');
			clearFilesFirst = (toupper(reply) == 'Y'); //clearFilesFirst = true when the user inputs Y
		}

		do
		{
			cout << "Would you like to run test cases to improve the AI's performance before \nplaying? (Y/N): ";
			cin >> reply;
		} while (toupper(reply) != 'Y' && toupper(reply) != 'N');
		realRun = (toupper(reply) == 'N'); //realRun = false when running testcases
	}

	if (clearFilesFirst)
	{
		ofstream file1("moves.txt");
		ofstream file2("boards.txt");
		file1.close();
		file2.close();
	}

	//Instructions
	if (realRun)
	{
		cout << "In this game, you'll input numbers from 1-9 to pick which square \nyou add your pieces in. The squares go horizonally " <<
			"then vertically. \nFor example, to pick the top right box you enter 3, and to pick \nthe lower left both you enter 7.\n\n" <<
			"Enjoy the game!\n\n" << endl;
	}

	const int numRounds = (realRun) ? 1 : numTrials;
	srand((unsigned)(time(0)));
	for (int counter = 0; counter < numRounds; counter++) //this for statement part should only be used when adding items to file
	{ //should be used in conjuction w/ random player moves function
		Board field;
		int prevMovePos = -1;
		int curMovePos = -1;
		fstream tempFile("moves.txt", ios::out | ios::ate | ios::in | ios::binary);
		fstream tempFile2("boards.txt", ios::out | ios::ate | ios::in | ios::binary);
		tempFile.close();
		tempFile2.close();
		for (int i = 0; i < 9; i++) //reset board
		{
			field.squares[i] = 0;
		}
		numBoards = countNumBoards();

		if (realRun)
			display(field);
		while (!(playerMove(field) || computerMove(field, curMovePos, prevMovePos)))
		{
			if (realRun)
			{
				system("cls");
				display(field);
			}
		}
		if (realRun)
		{
			system("cls");
			display(field);
		}

		if (victor == 1) //if computer loses, make the last move false
		{
			Move tempMove;
			fstream moves("moves.txt", ios::out | ios::in | ios::binary | ios::ate);
			moves.seekg(curMovePos * sizeof(Move), ios::beg);
			moves.read(reinterpret_cast<char *>(&tempMove), sizeof(Move));
			tempMove.works = false;
			moves.seekp(curMovePos * sizeof(Move), ios::beg);
			moves.write(reinterpret_cast<char *>(&tempMove), sizeof(Move));
			moves.close();
		
		}

		if (realRun)
		{
			if (victor == 1)
				cout << "YOU WIN!!!" << endl;
			else if (victor == -1)
				cout << "YOU LOST!!!" << endl;
			else
				cout << "TIE!!!" << endl;
		}
	
		int percentComplete = counter * 100.0 / numTrials;
		static int targetPercentage;
		if (counter == 0)
			targetPercentage = 5;
		if (!realRun)
		{
			if (percentComplete >= targetPercentage) //In intervals of 5
			{
				cout << percentComplete << "% complete" << endl;
				targetPercentage += 5;
			}

			if (counter == numTrials - 1)
			{
				cout << "Beginning the game! Press any button to continue!" << endl;
				system("pause");
				system("cls");
				clearFilesFirst = false;
				realRun = true;
				giveTestCaseOption = false;
				main();
			}
		}
			//cout << "Current iteration: " << counter << endl;
	}

	do
	{
		cout << "Would you like to play again (Y/N)? ";
		cin >> reply;
	} while (toupper(reply) != 'Y' && toupper(reply) != 'N');
	if (toupper(reply) == 'Y')
	{
		system("cls");
		clearFilesFirst = false;
		main();
	}

	return 0;
}

#pragma region BASIC FUNCTIONS

void display(const Board field)
{
	int i = 0;
	while (i < 9)
	{
		cout << setw(6) << right << "|" << setw(6) << right << "|" << endl;
		cout << setw(3) << right << getDisplayChar(field.squares[i++]) << setw(3) << right << "|";
		cout << setw(3) << right << getDisplayChar(field.squares[i++]) << setw(3) << right << "|";
		cout << setw(3) << right << getDisplayChar(field.squares[i++]) << endl;
		cout << setw(6) << right << "|" << setw(6) << right << "|" << endl;
		if (i != 9)
			for (int j = 0; j < 17; j++)
				cout << "-";
		cout << endl;
	}
}

char getDisplayChar(const int &num)
{
	if (num == 0)
		return ' ';
	else if (num == 1)
		return 'X';
	else if (num == -1)
		return 'O';
	else
	{
		cout << "ERROR: Invalid piece with num " << num << endl;
		throw "ERROR: INVALID PIECE";
	}
}

bool victory(const Board &field, int index)
{
	bool result = false;
	for (int i = 0; i < 3; i++)
	{
		if (field.squares[3 * i] + field.squares[3 * i + 1] + field.squares[3 * i + 2] == 3 * index) //horizontal
			result = true;
		else if (field.squares[i] + field.squares[i + 3] + field.squares[i + 6] == 3 * index) //vertical
			result = true;
	}
	if (!result)
	{
		if (field.squares[0] + field.squares[4] + field.squares[8] == 3 * index)
			result = true;
		if (field.squares[2] + field.squares[4] + field.squares[6] == 3 * index)
			result = true;
	}
	if (result)
	{
		victor = index;
		return true;
	}
	return false;
}

bool tie(const Board &field)
{
	for (int i = 0; i < 9; i++)
		if (field.squares[i] == 0)
			return false;
	return true;
}

bool operator==(const Board &one, const Board &two)
{
	for (int i = 0; i < 9; i++)
	{
		if (one.squares[i] != two.squares[i])
			return false;
	}
	return true;
}

#pragma endregion

#pragma region AI

bool playerMove(Board &field)
{
	if (tie(field))
	{
		victor = 0;
		return true; //victor = 0 -> tie
	}

	if (realRun)
	{
		int square;
		do
		{
			cout << "Enter a square (1-9): ";
			cin >> square;
		} while ((square < 1 || square > 9) || field.squares[square - 1] != 0);
		field.squares[square - 1] = 1;
	}
	else
	{
		int square;
		do
		{
			square = rand() % 9;
		} while (field.squares[square] != 0);
		field.squares[square] = 1;
	}
	
	return victory(field, 1);
}

bool computerMove(Board &field, int &curMovePos, int &prevMovePos)
{
	if (tie(field))
	{
		victor = 0; 
		return true; //victor = 0 -> tie
	}
	fstream boards("boards.txt", ios::out | ios::in | ios::binary | ios::ate);
	fstream moves("moves.txt", ios::out | ios::in | ios::binary | ios::ate);
	prevMovePos = curMovePos;
	curMovePos = findCurrentBoardLocationInFile(field);

	if (curMovePos == -1) //if board possibility is not in the file yet
	{
		Move tempMove = findNextMove(field);
		boards.seekp(0L, ios::end);
		boards.write(reinterpret_cast<char *>(&field), sizeof(Board));
		moves.seekp(0L, ios::end);
		moves.write(reinterpret_cast<char *>(&tempMove), sizeof(Move));
		curMovePos = numBoards;
		executeMove(field, tempMove, -1);
		numBoards++;
	}
	else //if board is in the file
	{
		Move tempMove;
		moves.seekg(curMovePos * sizeof(Move), ios::beg);
		moves.read(reinterpret_cast<char *>(&tempMove), sizeof(Move));
		if (tempMove.works) //if the move works
		{
			executeMove(field, tempMove, -1);
		}
		else //if the move doesn't work
		{
			Move nextMove = findNextMove(field, tempMove);
			if (nextMove.position == -1) //if there are no more potential moves
			{ 
				moves.seekg(prevMovePos * sizeof(Move), ios::beg);
				moves.read(reinterpret_cast<char *>(&nextMove), sizeof(Move));
				nextMove.works = false;
				moves.seekp(prevMovePos * sizeof(Move), ios::beg);
				moves.write(reinterpret_cast<char *>(&nextMove), sizeof(Move));

				executeMove(field, tempMove, -1);
			}
			else //if there are potential moves
			{
				moves.seekp(curMovePos * sizeof(Move), ios::beg);
				moves.write(reinterpret_cast<char *>(&nextMove), sizeof(Move));
				executeMove(field, nextMove, -1);
			}
		}
	}
	boards.close();
	moves.close();
	return victory(field, -1);
}

bool isMovePossible(const Board &field, const Move &move)
{
	return (field.squares[move.position] == 0); //if move is possible (square is empty) -> true
}

Move findNextMove(const Board &field) //should be removed ASAP
{
	Move move;
	for (int i = 0; i < 9; i++)
	{
		if (field.squares[i] == 0)
		{
			move.position = i;
			return move;
		}
			;
	}
	move.position = -1;
	return move;
}

Move findNextMove(const Board &field, const Move &move)
{
	Move nextMove;
	for (int i = move.position + 1; i < 9; i++)
	{
		nextMove.position = i;
		if (isMovePossible(field, nextMove))
			return nextMove;
	}

	nextMove.position = -1;
	return nextMove;
}

int findCurrentBoardLocationInFile(const Board &field)
{
	fstream boards("boards.txt", ios::out | ios::in | ios::binary | ios::ate);
	int count = 0;
	Board tempField;
	while (count < numBoards)
	{
		boards.seekg((static_cast<long>(count) * sizeof(Board)), ios::beg);
		boards.read(reinterpret_cast<char *>(&tempField), sizeof(Board));
		if (tempField == field)
			return count;
		count++;
	}
	boards.close();
	return -1;
}

void executeMove(Board &field, const Move &move, const int &index) //index by default -1
{
	field.squares[move.position] = index;
}

int countNumBoards() //check for errors (w/o any items -> length = -1)
{
	fstream boards("boards.txt", ios::out | ios::in | ios::binary | ios::ate);
	int numItems = 0;
	Board tempBoard;

	boards.seekg(0L, ios::beg);
	if (boards.peek() == EOF) //if file is empty
		return 0;
	while (!boards.eof())
	{
		boards.seekg(numItems * sizeof(Board), ios::beg);
		boards.read(reinterpret_cast<char *>(&tempBoard), sizeof(Board));
		if (!boards.eof())
			numItems++;
	}
	boards.close();
	return numItems;
}

#pragma endregion