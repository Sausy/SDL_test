#include "interface.h"
#include <Windows.h>

bool END;

void sdraw::decode_color(int rgb[]){
	rgb[1] = (rgb[0] & 0x00FF0000) >> 24;
	rgb[2] = (rgb[0] & 0x0000FF00) >> 24;
	rgb[0] = (rgb[0] & 0xFF000000) >> 24;

}

void keystroke(int *ctr_data) {
	while (!END) {

		if (GetKeyState(VK_UP) & 0x8000)
			*ctr_data = DOWN;
		if (GetKeyState(VK_DOWN) & 0x8000)
			*ctr_data = UP;
		if (GetKeyState(VK_RIGHT) & 0x8000)
			*ctr_data = RIGHT;
		if (GetKeyState(VK_LEFT) & 0x8000)
			*ctr_data = LEFT;

	}
}
