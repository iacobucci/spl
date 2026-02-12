#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rule.h"
#include "utils.h"

int RULE_ID = 0;
int RULE_DEPTH = 0;

rule *rule_c(char c) {
	rule *result = malloc(sizeof(rule));
	result->c = c;
	result->childs = NULL;
	result->id = RULE_ID++;
	result->n_childs = 0;
	result->parent = NULL;
	result->already_printed = 0;
	return result;
}

rule *rule_add_name(rule *r, char *name) {
	r->name = name;
	return r;
}

rule *rule_add_callback(rule *r, void (*callback)(char c), int where) {
	if (where == END)
		r->callback_end = callback;
	if (where == START)
		r->callback_start = callback;
	return r;
}

rule *rule_builder(int method, rule *r0, va_list args) {
	rule *result = malloc(sizeof(rule));
	result->method = method;
	result->id = RULE_ID++;
	result->n_childs = 0;

	result->c = '\0';

	result->already_printed = 0;

	int capacity = 4;
	result->childs = malloc(sizeof(rule *) * capacity);

	rule *r_arg = r0;

	while (r_arg != NULL) {
		if (result->n_childs >= capacity) {
			capacity *= 2;
			result->childs = realloc(result->childs, sizeof(rule *) * capacity);
		}

		r_arg->parent = result;
		result->childs[result->n_childs++] = r_arg;

		r_arg = va_arg(args, rule *);
	}

	return result;
}

rule *rule_or(rule *r0, ...) {
	va_list args;
	va_start(args, r0);

	rule *result = rule_builder(OR, r0, args);

	va_end(args);
	return result;
}

rule *rule_and(rule *r0, ...) {
	va_list args;
	va_start(args, r0);

	rule *result = rule_builder(AND, r0, args);

	va_end(args);
	return result;
}

rule *rule_range(char l1, char l2) {
	rule *result = malloc(sizeof(rule));

	result->method = OR;
	result->n_childs = l2 - l1 + 1;
	result->childs = malloc(sizeof(rule *) * result->n_childs);

	result->c = '\0';

	result->already_printed = 0;

	int i = 0;
	for (char c = l1; c < l2 + 1; c++, i++) {
		result->childs[i] = rule_c(c);
		result->childs[i]->parent = result;
	}

	return result;
}

rule *rule_literal(char *literal) {
	rule *result = malloc(sizeof(rule));

	result->method = AND;
	result->n_childs = strlen(literal);
	result->childs = malloc(sizeof(rule *) * result->n_childs);

	result->c = '\0';

	result->already_printed = 0;

	for (int i = 0; i < result->n_childs; i++) {
		result->childs[i] = rule_c(literal[i]);
		result->childs[i]->parent = result;
	}

	return result;
}

rule *rule_optional(rule *r) {
	rule *result = malloc(sizeof(rule));
	r->parent = result;

	result->method = OPTIONAL;
	result->id = RULE_ID++;

	result->n_childs = 1;
	result->childs = malloc(sizeof(rule *));
	result->childs[0] = r;

	result->c = '\0';

	result->already_printed = 0;

	return result;
}

rule *rule_zero_or_more(rule *r) {
	rule *result = malloc(sizeof(rule));
	r->parent = result;

	result->method = ZERO_OR_MORE;
	result->id = RULE_ID++;

	result->n_childs = 1;
	result->childs = malloc(sizeof(rule *));
	result->childs[0] = r;

	result->c = '\0';

	result->already_printed = 0;

	return result;
}

rule *rule_one_or_more(rule *r) {
	return rule_and(r, rule_zero_or_more(r), NULL);
}

char *rule_method_atob(int method) {
	switch (method) {
	case ZERO_OR_MORE:
		return "ZERO_OR_MORE";
	case OR:
		return "OR";
	case AND:
		return "AND";
	default:
		return "OPTIONAL";
	}
}

void rule_print_recursive(rule *r) {
	if (r == NULL)
		return;

	tabs_print(RULE_DEPTH);

	if (r->already_printed == 1) {
		printf("// ...\n");
		return;
	}

	if (r->name == NULL)
		printf("{ ");
	else
		printf("{ name: \"%s\", ", r->name);

	if (r->c != '\0') {
		printf("c: \"%c\"", r->c);
	} else {
		printf("method: \"%s\", definition: [\n", rule_method_atob(r->method));
	}

	RULE_DEPTH++;

	if (r->already_printed != 1) {
		r->already_printed = 1;
		for (int i = 0; i < r->n_childs; i++) {
			rule_print_recursive(r->childs[i]);
		}
	}

	RULE_DEPTH--;

	if (r->c == '\0') {
		tabs_print(RULE_DEPTH);
		printf("]}");
	} else {
		printf(" }");
	}

	if (RULE_DEPTH)
		printf(",");

	printf("\n");
}

void rule_clean_already_printed(rule *r) {
	if (r->already_printed != 0)
		for (int i = 0; i < r->n_childs; i++) {
			r->childs[i]->already_printed = 0;
			rule_clean_already_printed(r->childs[i]);
		}

	r->already_printed = 0;
}

void rule_print(rule *r) {
	rule_clean_already_printed(r);
	rule_print_recursive(r);
}

void rule_free(rule **rp) {
	rule *r = *rp;
	if (rp == NULL || r == NULL)
		return;

	for (int i = 0; i < r->n_childs; i++) {
		rule_free(&(r->childs[i]));
	}

	if (r->n_childs > 0)
		free(r->childs);

	free(r);

	*rp = NULL;
}

void rule_callback(rule *r, char c, int where) {
	if (where == END && r->callback_end != NULL)
		r->callback_end(c);
	if (where == START && r->callback_start != NULL)
		r->callback_start(c);
}
