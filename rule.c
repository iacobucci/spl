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

rule *rule_add_callback(rule *r, void (*callback)(ast *self)) {
	r->callback = callback;
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
		if (r->already_printed != 1) {
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

	if (r->already_printed != 1) {
		r->already_printed = 1;
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
	rule_print_recursive(r, 0);
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

void rule_callback(rule *r, ast *self) {
	if (r->callback != NULL)
		r->callback(self);
}
