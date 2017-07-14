#include "common.h"
#include "interface.h"
#include "objekt.h"

#include <thread>
#include <Windows.h>

#include "mcigraph.hpp"

#include "cfg.h"

void endprogramm(bool clc) {
	if (!clc)
		return void();
	g.running = FALSE;
	END = TRUE;
	for (int delay_i = 0; delay_i < 100; delay_i++) { ; }
}

void sdraw::rect(int pos[DIMENSION_DATA][DIMENSION][WINING_LENGTH+1], int *dim,int amount,int color) {
	int rgb[3] = { color,0,0 };
	decode_color(rgb);

	for (int i = 0; i < amount; i++) {
		if (pos[POSITION][X_data][i] == 0)
			break;
		else
			g.draw_rect(pos[POSITION][X_data][i], pos[POSITION][Y_data][i], dim[0], dim[1],false, rgb[0], rgb[1], rgb[2]);
	}
}



void f() { SDL_RenderDrawLine(g.ren, 0, 0, 100, 100); }


int main(int argc, char* argv[]) {
	
	END = FALSE;
	
	int score = 0;
	bool new_object = true;
	int amount_points = 1;
	
	int increasing_points[4] = { 3,7,16,32 };

	int ctr_data = 0;
	thread snake_t1(keystroke, &ctr_data);

	int game_area[2] = { LENGTH_X,HIGHT_Y }; // TODO: change with diverent screen

	sdraw draw;
	
	Objekt Snake;
	Objekt Points[5];
	Objekt* obj = &Snake;
	int Snake_pos_init[2] = { START_POS_SNAKE_X,START_POS_SNAKE_Y };
	int Points_pos_init[2] = { START_POS_POINTS_X,START_POS_POINTS_Y };
	Snake.init_data(Points_pos_init, UP, SNAKE_MOVEMENTSPEED,MIN_SNAKE_DIM);
	for (int i = 0; i < 5; i++) {
		Points[i].init_data(Snake_pos_init, 0, 0, MIN_POINTS_DIM);
		Points[i].color = POINTS_COLOR;
		Points[i].random_position(game_area);
	}
	Snake.color = SNAKE_COLOR;

	int amount_obj = 2;
		
	Objekt* ptr[30];
	ptr[0] = &Snake;
	for (int i = 0; i < 5; i++)
		ptr[1+i] = &Points[i];

	
	Collison col_points;

	int counter = 0;
	___MCILOOPSTART___

	SDL_SetRenderDrawColor(g.ren, 0, 0, 0, SDL_ALPHA_OPAQUE);

	
	for (int obj_counter = 0; obj_counter < amount_obj; obj_counter++) {
		draw.rect(ptr[obj_counter]->data, ptr[obj_counter]->dimension, ptr[obj_counter]->current_length, ptr[obj_counter]->color);
	}
	obj->move();
	obj->controll_data(ctr_data);

	//Check colision
	for (int obj_counter = 1; obj_counter < amount_obj; obj_counter++) {
		//point eaten 
		if (col_points.detect_decoded(obj->data, ptr[obj_counter]->data, obj->dimension, ptr[obj_counter]->dimension)) {
			//MessageBox(NULL, "collision", "collision",  MB_OK | MB_ICONQUESTION);
			obj->add_length();
			score++; new_object = true;
			if (score >= WIN)
				endprogramm(true);

			ptr[obj_counter]->random_position(game_area);
		}
	}
	//controll the points ... so that non of the points overlap
	for (int obj_counter = 1; obj_counter < amount_obj-1; obj_counter++) {
		if (col_points.detect_decoded(ptr[obj_counter]->data, ptr[obj_counter + 1]->data, ptr[obj_counter]->dimension, ptr[obj_counter + 1]->dimension)) {
			ptr[obj_counter]->random_position(game_area);
			ptr[obj_counter + 1]->data[POSITION][X_data][0] += 10+rand()%10;
		}
	}
	//look if snake bit itselfe
	for (int obj_member_count = 1; obj_member_count < Snake.current_length; obj_member_count++) {
		if (col_points.detect_decoded_snake(&Snake, obj_member_count)) {
			//change
		}
	}

	

	if (find(begin(increasing_points), end(increasing_points), score)!= end(increasing_points) && new_object && amount_points < 5) {
		Points[amount_points].init_data(Points_pos_init, 0, 0, MIN_POINTS_DIM);
		amount_obj++;
		amount_points++;
		ptr[amount_obj]->random_position(game_area);
		new_object = false;
		Snake.speed++;
	}

	___MCILOOPEND___

	snake_t1.detach();
	return 0;
}
