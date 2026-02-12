#pragma once
#include "rule.h"

enum parse_result_match { MATCHED, NOT_MATCHED };

typedef struct parse_result {
	int matched;
	char *remaining;
	char c;
} parse_result;

parse_result parse(char *text, rule *r);

void parse_assert_free(char *text, rule *r, int how);

void parse_assert(char *text, rule *r, int how);
