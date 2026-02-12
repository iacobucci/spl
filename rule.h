#pragma once

enum rule_method { ZERO_OR_MORE, AND, OR, OPTIONAL };

enum rule_callback_when { START, END };

typedef struct rule {
	int method;
	int id;
	char c;

	struct rule *parent;
	int n_childs;
	struct rule **childs;

	char *name;
	void (*callback_end)(char c);
	void (*callback_start)(char c);

	int already_printed;
} rule;

rule *rule_literal(char *literal);

rule *rule_c(char c);

rule *rule_range(char l1, char l2);

rule *rule_add_name(rule *r, char *name);

rule *rule_add_callback(rule *r, void (*callback)(char c), int where);

rule *rule_or(rule *r0, ...);

rule *rule_and(rule *r0, ...);

rule *rule_optional(rule *r);

rule *rule_zero_or_more(rule *r);

rule *rule_one_or_more(rule *r);

void rule_print(rule *r);

void rule_clean_already_printed(rule *r);

void rule_free(rule **rp);

void rule_callback(rule *r, char c, int where);
