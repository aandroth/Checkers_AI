#include "BEFNetwork.h"
#include <iostream>
using namespace std;

const int CHECKERS_BOARD_INPUTS = 32;

float BEF(int * board)
{
	int timer = 0;
	int runs = 1;

	float result;

	srand((unsigned int)time(0)); //Seed the random number generator

	while(1)
	{
		timer = (int)time(0);

		cout << endl << endl << runs << " runs DONE!\n";
		timer = (int)time(0) - timer;
		cin.sync();
		cin.get();
		system("cls");
	}

	return result;
}