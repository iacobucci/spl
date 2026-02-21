#include <stdio.h>

#include "utils.h"

void tabs_print(int depth) {
	for (int i = 0; i < depth; i++)
		printf("--");
}
