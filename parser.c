#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "parser.h"
#include "rule.h"

int PARSER_DEBUG = 0;

char *parse_result_atob(int result) {
	if (result == MATCHED)
		return "MATCHED";
	else
		return "NON_MATCHED";
}

parse_result parse_recursive(char *text, rule *r) {

	if (PARSER_DEBUG && text) {
		printf("[DEBUG] trying to parse text \"%s\" by extracting: ", text);
		rule_print(r);
	}

	parse_result result;
	result.node = NULL;
	ast *child;

	if (r->c != '\0') {
		if (text[0] != r->c) {
			// substring not found
			result.matched = NOT_MATCHED;
			result.remaining = text;
			result.c = '\0';
			return result;
		} else {
			// substring found
			result.matched = MATCHED;
			result.remaining = text + 1;
			result.c = text[0];

			result.node = ast_new("x", r->name);
			result.node->callback = r->callback;
			result.node->content[0] = result.c;
			result.node->ruleid = r->id;

			return result;
		}
	}
	if (r->method == OR) {
		char *original_text = text;

		for (int i = 0; i < r->n_childs; i++) {
			result = parse_recursive(text, r->childs[i]);

			if (result.matched == MATCHED) {

				child = result.node;
				result.node = ast_new(NULL, r->name);
				result.node->callback = r->callback;
				result.node->ruleid = r->id;
				ast_add_child(result.node, child);
				return result;
			}
		}

		result.matched = NOT_MATCHED;
		result.remaining = original_text;
		return result;
	}
	// one or more here is an hack to then reference rules by their variable
	// names.
	if (r->method == AND || r->method == ONE_OR_MORE) {
		int matched = MATCHED;
		char *remaining = text;
		char *original_text = text;

		ast *ast_and = ast_new(NULL, r->name);
		ast_and->callback = r->callback;
		ast_and->ruleid = r->id;

		for (int i = 0; i < r->n_childs; i++) {
			result = parse_recursive(remaining, r->childs[i]);

			if (result.matched != MATCHED) {
				result.remaining = original_text;
				ast_free(&ast_and);
				return result;
			}

			ast_add_child(ast_and, result.node);
			remaining = result.remaining;
		}

		result.remaining = remaining;
		result.node = ast_and;

		return result;
	}
	if (r->method == OPTIONAL) {
		rule *opt = r->childs[0];
		result = parse_recursive(text, opt);

		child = result.node;
		result.node = ast_new(NULL, r->name);
		result.node->callback = r->callback;
		result.node->ruleid = r->id;

		if (result.matched == MATCHED) {
			ast_add_child(result.node, child);
		}

		result.matched = MATCHED;

		return result;
	}
	if (r->method == ZERO_OR_MORE) {
		rule *zom = r->childs[0];

		ast *zom_ast = ast_new(NULL, r->name);
		zom_ast->callback = r->callback;
		zom_ast->ruleid = r->id;

		while (1) {
			result = parse_recursive(text, zom);
			text = result.remaining;

			if (result.matched != MATCHED) {
				break;
			} else {
				ast_add_child(zom_ast, result.node);
			}
		}

		result.matched = MATCHED;
		result.node = zom_ast;

		result.remaining = text;

		return result;
	}

	result.matched = NOT_MATCHED;
	result.remaining = text;

	return result;
}

parse_result parse(char *text, rule *r) {
	parse_result result = parse_recursive(text, r);
	if (strlen(result.remaining) != 0)
		result.matched = NOT_MATCHED;

	result.node = ast_get_root(result.node);

	return result;
}

void parse_result_free(parse_result pr) {
	pr.matched = NOT_MATCHED;
	ast_free(&(pr.node));
}

void parse_assert_free(char *text, rule *r, int how) {
	parse_result pr = parse(text, r);
	assert(pr.matched == how);
	parse_result_free(pr);
	rule_free(&r);
	if (PARSER_DEBUG)
		printf("\n");
}

void parse_assert(char *text, rule *r, int how) {
	parse_result pr = parse(text, r);
	assert(pr.matched == how);
	parse_result_free(pr);
	if (PARSER_DEBUG)
		printf("\n");
}
