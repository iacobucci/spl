#include <assert.h>
#include <stdio.h>
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

			result.node = ast_new("x");
			result.node->content[0] = result.c;

			rule_callback(r, result.c, END);

			return result;
		}
	}
	if (r->method == OR) {
		char *original_text = text;

		rule_callback(r, '\0', START);

		for (int i = 0; i < r->n_childs; i++) {
			result = parse_recursive(text, r->childs[i]);

			if (result.matched == MATCHED) {

				child = result.node;
				result.node = ast_new("or");
				ast_add_child(result.node, child);

				rule_callback(r, result.c, END);
				return result;
			}
		}
		result.matched = NOT_MATCHED;
		result.remaining = original_text;
		return result;
	}
	if (r->method == AND) {
		int matched = MATCHED;
		char *remaining = text;
		char *original_text = text;

		rule_callback(r, '\0', START);

		for (int i = 0; i < r->n_childs; i++) {
			result = parse_recursive(remaining, r->childs[i]);

			if (result.matched == NOT_MATCHED) {
				result.remaining = original_text;
				return result;
			}

			remaining = result.remaining;
		}

		child = result.node;
		result.node = ast_new("and");
		ast_add_child(result.node, child);

		rule_callback(r, result.c, END);

		result.remaining = remaining;
		return result;
	}
	if (r->method == OPTIONAL) {
		rule *opt = r->childs[0];
		result = parse_recursive(text, opt);

		rule_callback(r, '\0', START);

		if (result.matched == MATCHED) {
			child = result.node;
			result.node = ast_new("opt");
			ast_add_child(result.node, child);
			rule_callback(r, result.c, END);
		}
		result.matched = MATCHED;

		return result;
	}
	if (r->method == ZERO_OR_MORE) {
		rule *zom = r->childs[0];
		ast *zom_ast = ast_new("zom");

		rule_callback(r, '\0', START);

		while (1) {
			result = parse_recursive(text, zom);
			text = result.remaining;

			if (result.matched != MATCHED)
				break;
			else {
				ast_add_child(zom_ast, result.node);
				ast_print(zom_ast);
			}
		}

		rule_callback(r, result.c, END);

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

	return result;
}

void parse_assert_free(char *text, rule *r, int how) {
	assert(parse(text, r).matched == how);
	rule_free(&r);
	if (PARSER_DEBUG)
		printf("\n");
}

void parse_assert(char *text, rule *r, int how) {
	assert(parse(text, r).matched == how);
	if (PARSER_DEBUG)
		printf("\n");
}
