#include "json.h"
#include "parser.h"
#include "rule.h"
#include "utils.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ast {
	struct ast *parent;
	struct ast *child;
	struct ast *next;
	struct ast *prev;
	int n_childs;
	char *content;
} ast;

ast *ast_new(char *content) {
	ast *result = malloc(sizeof(ast));
	result->content = strdup(content);
	result->n_childs = 0;
	result->child = NULL;
	result->parent = NULL;
	result->next = NULL;
	result->prev = NULL;
	return result;
}

ast *ast_add_child(ast *parent, char *content) {
	ast *child = ast_new(content);
	child->parent = parent;
	parent->n_childs++;

	if (parent->n_childs == 1) {
		parent->child = child;
		return child;
	}

	ast *ith_child = parent->child;
	ast *prev_child = NULL;

	while (1) {
		if (ith_child->next == NULL) {
			ith_child->next = child;
			child->prev = prev_child;
			break;
		}
		ith_child = ith_child->next;
		prev_child = ith_child->prev;
	}

	return child;
}

void ast_free(ast **np) {
	if (np == NULL || *np == NULL) {
		return;
	}

	ast *node = *np;

	ast_free(&(node->next));
	ast_free(&(node->child));

	free(node->content);
	free(node);

	*np = NULL;
}

void ast_print_recursive(ast *node, int depth, int print_next) {
	while (node != NULL) {
		tabs_print(depth);
		printf("%s\n", node->content);

		if (node->child != NULL)
			ast_print_recursive(node->child, depth + 1, 1);

		if (print_next == 0)
			return;

		node = node->next;
	}
}

void ast_print(ast *node) { ast_print_recursive(node, 0, 0); }

int main() {

	ast *ciao = ast_new("ciao");
	ast *cane = ast_add_child(ciao, "cane");
	ast *casa = ast_add_child(ciao, "casa");
	ast *capra = ast_add_child(casa, "capra");
	ast *cavallo = ast_add_child(ciao, "cavallo");

	// ast_print(ciao);
	// ast_free(&ciao);

	json_init();

	parse_result pr = parse("[\"ciao\", false, \"cane\"]", value);

	printf("\n\n");

	return 0;
}
