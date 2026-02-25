#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void tabs_print(int depth) {
	for (int i = 0; i < depth; i++)
		printf("  ");
}

void error_check_malloc(void *p) {
	if (p == NULL) {
		perror("malloc failed");
		exit(-1);
	}
}

char *string_concat(const char *s1, const char *s2) {
	char *result = malloc(strlen(s1) + strlen(s2) + 1);

	error_check_malloc(result);

	strcpy(result, s1);
	strcat(result, s2);
	return result;
}
