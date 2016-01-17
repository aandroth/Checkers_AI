
#ifndef MOVERESULTSARRAY_H
#define MOVERESULTSARRAY_H

//the struct to hold the values in the lookup tables
struct cells
{
	int left;
	int right;
};

class MoveResultArray
{
public:
	//lookup table for up moves
	cells movesUp[32];

	//lookup table for down moves
	cells movesDown[32];

	//lookup table for up jumps
	cells jumpsUp[32];

	//lookup table for down jumps
	cells jumpsDown[32];

	MoveResultArray(){};
	void assignTables();
};


//assignTables
//
//Function assigns the look up tables for the moves and jumps
//
//Pre:	None
//Post: Assigns values to all of the cells of the tables.
//
//look-up table for moves
//##00##01##02##03
//04##05##06##07##
//##08##09##10##11
//12##13##14##15##
//##16##17##18##19
//20##21##22##23##
//##24##25##26##27
//28##29##30##31##
//
void MoveResultArray::assignTables()
{
	//look-up table for up moves
	movesUp[0].left = -1;
	movesUp[0].right = -1;

	movesUp[1].left = -1;
	movesUp[1].right = -1;

	movesUp[2].left = -1;
	movesUp[2].right = -1;

	movesUp[3].left = -1;
	movesUp[3].right = -1;

	movesUp[4].left = -1;
	movesUp[4].right = 0;

	movesUp[5].left = 0;
	movesUp[5].right = 1;

	movesUp[6].left = 1;
	movesUp[6].right = 2;

	movesUp[7].left = 2;
	movesUp[7].right = 3;

	movesUp[8].left = 4;
	movesUp[8].right = 5;

	movesUp[9].left = 5;
	movesUp[9].right = 6;

	movesUp[10].left = 6;
	movesUp[10].right = 7;

	movesUp[11].left = 7;
	movesUp[11].right = -1;

	movesUp[12].left = -1;
	movesUp[12].right = 8;

	movesUp[13].left = 8;
	movesUp[13].right = 9;

	movesUp[14].left = 9;
	movesUp[14].right = 10;

	movesUp[15].left = 10;
	movesUp[15].right = 11;

	movesUp[16].left = 12;
	movesUp[16].right = 13;

	movesUp[17].left = 13;
	movesUp[17].right = 14;

	movesUp[18].left = 14;
	movesUp[18].right = 15;

	movesUp[19].left = 15;
	movesUp[19].right = -1;

	movesUp[20].left = -1;
	movesUp[20].right = 16;

	movesUp[21].left = 16;
	movesUp[21].right = 17;

	movesUp[22].left = 17;
	movesUp[22].right = 18;

	movesUp[23].left = 18;
	movesUp[23].right = 19;

	movesUp[24].left = 20;
	movesUp[24].right = 21;

	movesUp[25].left = 21;
	movesUp[25].right = 22;

	movesUp[26].left = 22;
	movesUp[26].right = 23;

	movesUp[27].left = 23;
	movesUp[27].right = -1;

	movesUp[28].left = -1;
	movesUp[28].right = 24;

	movesUp[29].left = 24;
	movesUp[29].right = 25;

	movesUp[30].left = 25;
	movesUp[30].right = 26;

	movesUp[31].left = 26;
	movesUp[31].right = 27;

	//look-up table for down moves
	movesDown[0].left = 4;
	movesDown[0].right = 5;

	movesDown[1].left = 5;
	movesDown[1].right = 6;

	movesDown[2].left = 6;
	movesDown[2].right = 7;

	movesDown[3].left = 7;
	movesDown[3].right = -1;

	movesDown[4].left = -1;
	movesDown[4].right = 8;

	movesDown[5].left = 8;
	movesDown[5].right = 9;

	movesDown[6].left = 9;
	movesDown[6].right = 10;

	movesDown[7].left = 10;
	movesDown[7].right = 11;

	movesDown[8].left = 12;
	movesDown[8].right = 13;

	movesDown[9].left = 13;
	movesDown[9].right = 14;

	movesDown[10].left = 14;
	movesDown[10].right = 15;

	movesDown[11].left = 15;
	movesDown[11].right = -1;

	movesDown[12].left = -1;
	movesDown[12].right = 16;

	movesDown[13].left = 16;
	movesDown[13].right = 17;

	movesDown[14].left = 17;
	movesDown[14].right = 18;

	movesDown[15].left = 18;
	movesDown[15].right = 19;

	movesDown[16].left = 20;
	movesDown[16].right = 21;

	movesDown[17].left = 21;
	movesDown[17].right = 22;

	movesDown[18].left = 22;
	movesDown[18].right = 23;

	movesDown[19].left = 23;
	movesDown[19].right = -1;

	movesDown[20].left = -1;
	movesDown[20].right = 24;

	movesDown[21].left = 24;
	movesDown[21].right = 25;

	movesDown[22].left = 25;
	movesDown[22].right = 26;

	movesDown[23].left = 26;
	movesDown[23].right = 27;

	movesDown[24].left = 28;
	movesDown[24].right = 29;

	movesDown[25].left = 29;
	movesDown[25].right = 30;

	movesDown[26].left = 30;
	movesDown[26].right = 31;

	movesDown[27].left = 31;
	movesDown[27].right = -1;

	movesDown[28].left = -1;
	movesDown[28].right = -1;

	movesDown[29].left = -1;
	movesDown[29].right = -1;

	movesDown[30].left = -1;
	movesDown[30].right = -1;

	movesDown[31].left = -1;
	movesDown[31].right = -1;


	//look-up table for up jumps
	jumpsUp[0].left = -1;
	jumpsUp[0].right = -1;

	jumpsUp[1].left = -1;
	jumpsUp[1].right = -1;

	jumpsUp[2].left = -1;
	jumpsUp[2].right = -1;

	jumpsUp[3].left = -1;
	jumpsUp[3].right = -1;

	jumpsUp[4].left = -1;
	jumpsUp[4].right = -1;

	jumpsUp[5].left = -1;
	jumpsUp[5].right = 1;

	jumpsUp[6].left = -1;
	jumpsUp[6].right = -1;

	jumpsUp[7].left = -1;
	jumpsUp[7].right = -1;

	jumpsUp[8].left = -1;
	jumpsUp[8].right = 1;

	jumpsUp[9].left = 0;
	jumpsUp[9].right = 2;

	jumpsUp[10].left = 1;
	jumpsUp[10].right = 3;

	jumpsUp[11].left = 2;
	jumpsUp[11].right = -1;

	jumpsUp[12].left = -1;
	jumpsUp[12].right = 5;

	jumpsUp[13].left = 4;
	jumpsUp[13].right = 6;

	jumpsUp[14].left = 5;
	jumpsUp[14].right = 7;

	jumpsUp[15].left = 6;
	jumpsUp[15].right = -1;

	jumpsUp[16].left = -1;
	jumpsUp[16].right = 9;

	jumpsUp[17].left = 8;
	jumpsUp[17].right = 10;

	jumpsUp[18].left = 9;
	jumpsUp[18].right = 11;

	jumpsUp[19].left = 10;
	jumpsUp[19].right = -1;

	jumpsUp[20].left = -1;
	jumpsUp[20].right = 13;

	jumpsUp[21].left = 12;
	jumpsUp[21].right = 14;

	jumpsUp[22].left = 13;
	jumpsUp[22].right = 15;

	jumpsUp[23].left = 14;
	jumpsUp[23].right = -1;

	jumpsUp[24].left = -1;
	jumpsUp[24].right = 17;

	jumpsUp[25].left = 16;
	jumpsUp[25].right = 18;

	jumpsUp[26].left = 17;
	jumpsUp[26].right = 19;

	jumpsUp[27].left = 18;
	jumpsUp[27].right = -1;

	jumpsUp[28].left = -1;
	jumpsUp[28].right = 21;

	jumpsUp[29].left = 20;
	jumpsUp[29].right = 22;

	jumpsUp[30].left = 21;
	jumpsUp[30].right = 23;

	jumpsUp[31].left = 22;
	jumpsUp[31].right = -1;

	//look-up table for down jumps
	jumpsDown[0].left = -1;
	jumpsDown[0].right = 9;

	jumpsDown[1].left = 8;
	jumpsDown[1].right = 10;

	jumpsDown[2].left = 9;
	jumpsDown[2].right = 11;

	jumpsDown[3].left = 10;
	jumpsDown[3].right = -1;

	jumpsDown[4].left = -1;
	jumpsDown[4].right = 13;

	jumpsDown[5].left = 12;
	jumpsDown[5].right = 14;

	jumpsDown[6].left = 13;
	jumpsDown[6].right = 15;

	jumpsDown[7].left = 14;
	jumpsDown[7].right = -1;

	jumpsDown[8].left = -1;
	jumpsDown[8].right = 17;

	jumpsDown[9].left = 16;
	jumpsDown[9].right = 18;

	jumpsDown[10].left = 17;
	jumpsDown[10].right = 19;

	jumpsDown[11].left = 18;
	jumpsDown[11].right = -1;

	jumpsDown[12].left = -1;
	jumpsDown[12].right = 21;

	jumpsDown[13].left = 20;
	jumpsDown[13].right = 22;

	jumpsDown[14].left = 21;
	jumpsDown[14].right = 23;

	jumpsDown[15].left = 22;
	jumpsDown[15].right = -1;

	jumpsDown[16].left = -1;
	jumpsDown[16].right = 25;

	jumpsDown[17].left = 24;
	jumpsDown[17].right = 26;

	jumpsDown[18].left = 25;
	jumpsDown[18].right = 27;

	jumpsDown[19].left = 26;
	jumpsDown[19].right = -1;

	jumpsDown[20].left = -1;
	jumpsDown[20].right = 29;

	jumpsDown[21].left = 28;
	jumpsDown[21].right = 30;

	jumpsDown[22].left = 29;
	jumpsDown[22].right = 31;

	jumpsDown[23].left = 30;
	jumpsDown[23].right = -1;

	jumpsDown[24].left = -1;
	jumpsDown[24].right = -1;

	jumpsDown[25].left = -1;
	jumpsDown[25].right = -1;

	jumpsDown[26].left = -1;
	jumpsDown[26].right = -1;

	jumpsDown[27].left = -1;
	jumpsDown[27].right = -1;

	jumpsDown[28].left = -1;
	jumpsDown[28].right = -1;

	jumpsDown[29].left = -1;
	jumpsDown[29].right = -1;

	jumpsDown[30].left = -1;
	jumpsDown[30].right = -1;

	jumpsDown[31].left = -1;
	jumpsDown[31].right = -1;

}

#endif //MOVERESULTSARRAY