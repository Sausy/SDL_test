#pragma once
#include "common.h"

extern bool END;

class sdraw {
public:
	void rect(int pos[DIMENSION_DATA][DIMENSION][WINING_LENGTH + 1], int *dim,int amount,int color);
	void decode_color(int rgb[]);
};

void keystroke(int *ctr_data);