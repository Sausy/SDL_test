#include "objekt.h"
#include <math.h>
#include <cstdlib>
#include <ctime>

void Objekt::init_data(int pos[], int direction,int mvspeed, int mindimesion) {
	current_length = INIT_LENGTH;

	speed = mvspeed;
	dimension[0] = mindimesion;
	dimension[1] = mindimesion;

	if (abs(direction) > 1) 
		data[MOVEMENT][Y_data][0] = direction;
	else 
		data[MOVEMENT][X_data][0] = direction;
	
	data[POSITION][X_data][0] = pos[X_data];
	data[POSITION][Y_data][0] = pos[Y_data];

	Objekt::move();
}

void Objekt::add_length() {

	for (int i = 0; i < 8; i++) {
		data[POSITION][X_data][current_length + 1] = data[POSITION][X_data][current_length];
		data[POSITION][Y_data][current_length + 1] = data[POSITION][Y_data][current_length];
		current_length += 1;
	}
}

void Objekt::move() {
	data[POSITION][X_data][current_length] = NULL;
	data[POSITION][Y_data][current_length] = NULL;

	for (int count_i = current_length-1; count_i >= 0; count_i--) {
		data[POSITION][X_data][count_i+1] = data[POSITION][X_data][count_i];
		data[POSITION][Y_data][count_i + 1] = data[POSITION][Y_data][count_i];
	}
	
	data[POSITION][X_data][0] += data[MOVEMENT][X_data][0]*speed;
	data[POSITION][Y_data][0] += data[MOVEMENT][Y_data][0]/2*speed;

}


void Objekt::random_position(int dim[]) {
	srand(time(NULL));
	data[POSITION][X_data][0] = random(0, dim[X_data]);
	data[POSITION][Y_data][0] = random(0, dim[Y_data]);
}

void Objekt::controll_data(int direction) {
	if (abs(direction) > 1 && abs(data[MOVEMENT][Y_data][0]) == 0) {
		data[MOVEMENT][Y_data][0] = direction;
		data[MOVEMENT][X_data][0] = 0;
	}
	else if (abs(direction) == 1 && abs(data[MOVEMENT][X_data][0]) == 0) {
		data[MOVEMENT][X_data][0] = direction;
		data[MOVEMENT][Y_data][0] = 0;
	}
	
}

bool Collison::detect(int **pos, int **dim) {
	double r[2];

	int colisionsbuffer = 9;

	for (int i = 0; i < 2; i++) {
		if (dim[i][X_data] >= dim[i][Y_data])
			r[i] = dim[i][Y_data]/2;
		else
			r[i] = dim[i][X_data]/2;
	}

	int x = pos[0][X_data] - pos[1][X_data];
	int y = pos[0][Y_data] - pos[1][Y_data];
	double colision = sqrt(pow(x,2)+pow(y,2));


	colision -= r[0] + r[1] - colisionsbuffer;

	if (colision > 0)
		return false;
	else
		return true;
}

bool Collison::detect_decoded(int pos1[DIMENSION_DATA][DIMENSION][WINING_LENGTH + 1], int pos2[DIMENSION_DATA][DIMENSION][WINING_LENGTH + 1], int *dim1, int *dim2) {
	int **pos = new int *[2];
	int **dim = new int *[2];

	int i;
	for (i = 0; i < 2; i++) {
		pos[i] = new int[2];
		dim[i] = new int[2];
	}

	pos[0][0] = pos1[POSITION][X_data][0];
	pos[0][1] = pos1[POSITION][Y_data][0];
	pos[1][0] = pos2[POSITION][X_data][0];
	pos[1][1] = pos2[POSITION][Y_data][0];

	dim[0][0] = dim1[X_data];
	dim[0][1] = dim1[Y_data];
	dim[1][0] = dim2[X_data];
	dim[1][1] = dim2[Y_data];

	return detect(pos,dim);
}

bool Collison::detect_decoded_snake(Objekt *temp_obj, int segment) {
	int **pos = new int *[2];
	int **dim = new int *[2];

	int i;
	for (i = 0; i < 2; i++) {
		pos[i] = new int[2];
		dim[i] = new int[2];
	}

	pos[0][0] = temp_obj->data[POSITION][X_data][0];
	pos[0][1] = temp_obj->data[POSITION][Y_data][0];
	pos[1][0] = temp_obj->data[POSITION][X_data][segment];
	pos[1][1] = temp_obj->data[POSITION][Y_data][segment];

	dim[0][0] = temp_obj->dimension[X_data];
	dim[0][1] = temp_obj->dimension[Y_data];
	dim[1][0] = temp_obj->dimension[X_data];
	dim[1][1] = temp_obj->dimension[Y_data];


	return detect(pos, dim);
}

