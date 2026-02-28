#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rule.h"
#include "utils.h"

int RULE_ID = 0;
int RULE_DEPTH = 0;

int rule_new_id() { return RULE_ID++; }

rule *rule_new() {
	rule *result = malloc(sizeof(rule));
	error_check_malloc(result);

	result->c = '\0';
	result->childs = NULL;
	result->id = rule_new_id();
	result->n_childs = 0;
	result->parent = NULL;

	result->repeats = 1;
	result->visited = 0;
	return result;
}

rule *rule_c(char c) {
	rule *result = rule_new();
	result->c = c;
	return result;
}

rule *rule_add_name(rule *r, char *name) {
	r->name = name;
	return r;
}

rule *rule_add_callback(rule *r, void (*callback)(ast *self)) {
	r->callback = callback;
	return r;
}

rule *rule_builder(int method, rule *r0, va_list args) {
	rule *result = rule_new();
	result->method = method;

	int capacity = 4;
	result->childs = malloc(sizeof(rule *) * capacity);
	error_check_malloc(result->childs);

	rule *r_arg = r0;

	int i = 0;
	while (r_arg != NULL) {
		result->n_childs += r_arg->repeats;

		while (result->n_childs >= capacity) {
			capacity *= 2;
			result->childs = realloc(result->childs, sizeof(rule *) * capacity);
			error_check_malloc(result->childs);
		}

		r_arg->parent = result;

		for (int j = 0; j < r_arg->repeats; j++) {
			result->childs[i] = r_arg;
			i++;
		}

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

rule *rule_range(char c1, char c2) {
	rule *result = rule_new();

	result->method = OR;
	result->n_childs = c2 - c1 + 1;
	result->childs = malloc(sizeof(rule *) * result->n_childs);
	error_check_malloc(result->childs);

	int i = 0;
	for (char c = c1; c < c2 + 1; c++, i++) {
		result->childs[i] = rule_c(c);
		result->childs[i]->parent = result;
	}

	return result;
}

rule *rule_literal(char *literal) {
	rule *result = rule_new();
	result->method = AND;
	result->n_childs = strlen(literal);
	result->childs = malloc(sizeof(rule *) * result->n_childs);
	error_check_malloc(result->childs);

	for (int i = 0; i < result->n_childs; i++) {
		result->childs[i] = rule_c(literal[i]);
		result->childs[i]->parent = result;
	}

	return result;
}

rule *rule_optional(rule *r) {
	rule *result = rule_new();

	result->method = OPTIONAL;
	r->parent = result;
	result->n_childs = 1;
	result->childs = malloc(sizeof(rule *));
	error_check_malloc(result->childs);
	result->childs[0] = r;

	return result;
}

rule *rule_repeat(rule *r, int times) {
	r->repeats = times;
	return r;
}

rule *rule_zero_or_more(rule *r) {
	rule *result = rule_new();

	r->parent = result;
	result->method = ZERO_OR_MORE;
	result->n_childs = 1;
	result->childs = malloc(sizeof(rule *));
	error_check_malloc(result->childs);
	result->childs[0] = r;

	return result;
}

rule *rule_one_or_more(rule *r) {
	rule *result = rule_and(r, rule_zero_or_more(r), NULL);
	result->method = ONE_OR_MORE;
	return result;
}

char *rule_method_atob(int method) {
	switch (method) {
	case ZERO_OR_MORE:
		return "ZERO_OR_MORE";
	case OR:
		return "OR";
	case AND:
		return "AND";
	case ONE_OR_MORE:
		return "AND";
	default:
		return "OPTIONAL";
	}
}

void rule_print_recursive(rule *r, int print_comma) {
	if (r == NULL)
		return;

	tabs_print(RULE_DEPTH);

	if (r->name == NULL)
		printf("{ ");
	else
		printf("{ \"name\": \"%s\", ", r->name);

	if (r->c != '\0') {
		char c = r->c;
		switch (c) {
		case '\t':
			printf("\"c\": \"\\t\"");
			break;
		case '\n':
			printf("\"c\": \"\\n\"");
			break;
		case '\r':
			printf("\"c\": \"\\r\"");
			break;
		case '\b':
			printf("\"c\": \"\\b\"");
			break;
		case '\f':
			printf("\"c\": \"\\f\"");
			break;
		case '\"':
			printf("\"c\": \"\\\"\"");
			break;
		case '\\':
			printf("\"c\": \"\\\\\"");
			break;
		default:
			printf("\"c\": \"%c\"", c);
		}

	} else {
		printf("\"method\": \"%s\", ", rule_method_atob(r->method));
		if (r->visited != 1) {
			printf("\"definition\": [\n");
		} else {
			printf("\"already_printed\": true }");

			if (print_comma)
				printf(",");

			printf("\n");

			return;
		}
	}

	RULE_DEPTH++;

	if (r->visited != 1) {
		r->visited = 1;
		for (int i = 0; i < r->n_childs; i++) {
			rule_print_recursive(r->childs[i], (i + 1 < r->n_childs));
		}
	}

	RULE_DEPTH--;

	if (r->c == '\0') {
		tabs_print(RULE_DEPTH);
		printf("]}");
	} else {
		printf(" }");
	}

	if (print_comma)
		printf(",");

	printf("\n");
}

void rule_clean_visited(rule *r) {
	if (r == NULL)
		return;

	if (r->visited != 0)
		for (int i = 0; i < r->n_childs; i++) {
			r->childs[i]->visited = 0;
			rule_clean_visited(r->childs[i]);
		}

	r->visited = 0;
}

void rule_print(rule *r) {
	rule_clean_visited(r);
	rule_print_recursive(r, 0);
}

void rule_free_recursive(rule **rp) {
	if (rp == NULL)
		return;

	rule *r = *rp;

	if (r == NULL)
		return;

	if (r->visited)
		return;

	r->visited = 1;

	for (int i = 0; i < r->n_childs; i++) {
		rule_free_recursive(&(r->childs[i]));
	}

	if (r->n_childs > 0 && r->childs != NULL) {
		free(r->childs);
	}

	if (r != NULL)
		free(r);

	*rp = NULL;
}

void rule_free(rule **rp) {
	rule_clean_visited(*rp);
	rule_free_recursive(rp);
}

void rule_callback(rule *r, ast *self) {
	if (r->callback != NULL)
		r->callback(self);
}
