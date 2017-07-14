#include "common.h"

int random(int low, int high) {
	if (low > high)return high;
	int k = rand() + rand();
	k = rand() % k;
	k += rand() * rand()%3;
	return low + (k % (high - low + 1));
}