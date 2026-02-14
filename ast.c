#include "ast.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

ast *ast_add_child(ast *parent, ast *child) {
	if (child == NULL)
		return parent;

	child->parent = parent;
	parent->n_childs++;

	if (parent->n_childs == 1) {
		parent->child = child;
		return child;
	}

	ast *ith_child = parent->child;
	ast *prev_child = NULL;

	while (1) {
		prev_child = ith_child;
		if (ith_child->next == NULL) {
			ith_child->next = child;
			child->prev = prev_child;
			break;
		}
		ith_child = ith_child->next;
	}

	return child;
}

ast *ast_add_sibling(ast *node, ast *child) {
	if (node == NULL)
		return child;
	if (child == NULL)
		return node;

	ast *prev = NULL;

	while (node->next != NULL) {
		node = node->next;
		prev = node;
	}

	node->next = child;
	node->prev = prev;

	return child;
}

void ast_collapse_only_childs_recursive(ast *node) {
	if (node == NULL)
		return;

	ast_collapse_only_childs_recursive(node->child);
	ast_collapse_only_childs_recursive(node->next);

	if (node->next == NULL && node->prev == NULL) {
		ast *parent = node->parent;
		if (parent != NULL) {
			ast *grandparent = parent->parent;
			node->parent = grandparent;
			printf("leaf: %s -> %s\n", parent->content, node->content);
		}
	}
}

ast *ast_collapse_only_childs(ast *node) {
	ast_collapse_only_childs_recursive(node);
	return node;
}

ast *ast_pop(ast *node) {
	ast *temp = node;
	while (1) {
		if (temp->next == NULL)
			break;

		if (temp->next->next == NULL) {
			ast_free(&temp->next);
			break;
		}

		temp = temp->next;
	}

	return node;
}

ast *ast_get_root(ast *node) {
	while (node->parent != NULL)
		node = node->parent;
	return node;
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

void ast_print(ast *node) { ast_print_recursive(node, 0, 1); }
