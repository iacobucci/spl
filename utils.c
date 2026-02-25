#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define UTIL_TAB "  "

void tabs_print(int depth) {
	for (int i = 0; i < depth; i++)
		printf(UTIL_TAB);
}

char *tabs_to_string(int depth) {
	int tablen = sizeof(UTIL_TAB) - 1;
	char *result = malloc(sizeof(char) * (tablen * depth + 1));

	for (int i = 0; i < depth; i++)
		for (int j = 0; j < tablen; j++)
			result[(i * tablen) + j] = UTIL_TAB[j];

	result[depth * tablen] = '\0';
	return result;
}

void error_check_malloc(void *p) {
	if (p == NULL) {
		perror("malloc failed");
		exit(-1);
	}
}

char *string_concat(const char *s1, const char *s2) {
	if (s1 != NULL && s2 != NULL) {
		char *result = malloc(sizeof(char) * (strlen(s1) + strlen(s2) + 1));
		error_check_malloc(result);

		strcpy(result, s1);
		strcat(result, s2);
		return result;
	}

	if (s1 == NULL && s2 == NULL)
		return strdup("");

	if (s1 == NULL)
		return strdup(s2);

	if (s2 == NULL)
		return strdup(s1);

	return NULL;
}
