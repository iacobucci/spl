#pragma once
#include "ast.h"

enum rule_method { ZERO_OR_MORE, ONE_OR_MORE, AND, OR, OPTIONAL };

#ifndef STRUCT_RULE
#define STRUCT_RULE

struct ast;

typedef struct rule {
	int method;
	int id;
	char c;

	struct rule *parent;
	int n_childs;
	struct rule **childs;

	char *name;
	void (*callback)(struct ast *self);

	int visited;
} rule;

#endif

int rule_new_id();

rule *rule_literal(char *literal);

rule *rule_c(char c);

rule *rule_range(char l1, char l2);

rule *rule_add_name(rule *r, char *name);

rule *rule_add_callback(rule *r, void (*callback)(ast *self));

rule *rule_or(rule *r0, ...);

rule *rule_and(rule *r0, ...);

rule *rule_optional(rule *r);

rule *rule_zero_or_more(rule *r);

rule *rule_one_or_more(rule *r);

char *rule_to_string(rule *r);

void rule_print(rule *r);

void rule_clean_already_printed(rule *r);

void rule_free(rule **rp);

void rule_callback(rule *r, ast *self);
