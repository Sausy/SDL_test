#pragma once
#include "common.h"


class Objekt {
public:
	void move();
	void controll_data(int direction);
	void init_data(int pos[], int direction, int mvspeed, int mindimesion);
	void add_length();
	void random_position(int dim[]);

	int dimension[2];

	int current_length;

	int color = 0;

	signed int speed;

	//contains the position an direktion from every part of the objekt 
	//first [data type][coordinates][every element from the Snake]
	int data[DIMENSION_DATA][DIMENSION][WINING_LENGTH + 1] = { 0 };
private:
	/*-------------------obsoleted-----
	//kinks 
	//contains the postion of the kinks and the v_vektor change data;
	int kink[DIMENSION_DATA][DIMENSION][MAX_DATA];
	*/
};


class Collison {
public:
	bool detect(int **pos, int **dim, int *direc);
	bool detect_decoded(int pos1[DIMENSION_DATA][DIMENSION][WINING_LENGTH + 1], int pos2[DIMENSION_DATA][DIMENSION][WINING_LENGTH + 1], int *dim1, int *dim2);
	bool detect_decoded_snake(Objekt *temp_obj, int segment);
	bool collided;
};