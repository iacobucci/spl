#include "ast.h"
#include "rule.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ast *ast_new(char *content, char *name) {
	ast *result = malloc(sizeof(ast));

	if (content != NULL)
		result->content = strdup(content);
	else
		result->content = NULL;

	if (name != NULL)
		result->name = strdup(name);
	else
		result->name = NULL;

	result->n_childs = 0;
	result->child = NULL;
	result->parent = NULL;
	result->next = NULL;
	result->prev = NULL;

	result->ruleid = -1;
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

void ast_free_individual_node(ast *n) {
	if (n == NULL)
		return;

	if (n->content != NULL)
		free(n->content);

	if (n->name != NULL)
		free(n->name);

	free(n);
}

void ast_simplify(ast *node, rule *r) {
	if (node == NULL || r == NULL)
		return;

	if (node->ruleid == r->id) {
		printf("RULE TO SIMPLIFY\n");
	}

	ast_simplify(node->child, r);
	ast_simplify(node->next, r);
}

void ast_wipe(ast *node, rule *r) {
	if (node == NULL)
		return;

	if (node->child != NULL) {
		if (node->child->ruleid == r->id) {
			ast *old_child = node->child;
			node->child = node->child->next;

			ast_free_individual_node(old_child);
		}
	}

	if (node->ruleid != r->id)
		ast_wipe(node->child, r);

	if (node->ruleid == r->id) {
		if (node->prev != NULL) {
			node->prev->next = node->next;
		}
		if (node->next != NULL)
			node->next->prev = node->prev;

		ast_free_individual_node(node);
	}

	if (node->next != NULL)
		ast_wipe(node->next, r);
}

int collapsing_to_add_index;
int to_add_buffer_length;

#define TO_ADD_INITIAL_BUFFER_LENGTH 4

void ast_collapse_recursive(ast *node, rule *r, ast *to_add,
							char *to_add_buffer, int collapsing) {
	if (node == NULL)
		return;

	if (collapsing && node->content) {

		to_add_buffer[collapsing_to_add_index] = node->content[0];
		collapsing_to_add_index++;

		if (collapsing_to_add_index == to_add_buffer_length) {
			to_add_buffer_length *= 2;
			to_add->content = realloc(
				to_add_buffer, sizeof(char) * (to_add_buffer_length + 1));
		}

		to_add_buffer[collapsing_to_add_index + 1] = '\0';
	}

	if (node->ruleid == r->id) {
		to_add_buffer =
			malloc(sizeof(char) * (TO_ADD_INITIAL_BUFFER_LENGTH + 1));
		to_add_buffer_length = TO_ADD_INITIAL_BUFFER_LENGTH;

		node->content = to_add_buffer;
		to_add = node;

		collapsing_to_add_index = 0;
		collapsing = 1;
	}

	ast_collapse_recursive(node->child, r, to_add, to_add_buffer, collapsing);
	ast_collapse_recursive(node->next, r, to_add, to_add_buffer, collapsing);

	if (node->ruleid == r->id) {
		if (node->child)
			ast_free(&(node->child));
	}
}

void ast_collapse(ast *node, rule *r) {
	ast_collapse_recursive(node, r, NULL, NULL, 0);
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
	if (node == NULL)
		return NULL;

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

	ast_free_individual_node(node);

	*np = NULL;
}

void ast_print_recursive(ast *node, int depth, int print_next) {
	while (node != NULL) {
		tabs_print(depth);

		if (node->content != NULL)
			printf("%s ", node->content);
		if (node->name)
			printf("%s ", node->name);
		if (node->ruleid)
			printf("%d", node->ruleid);
		printf("\n");

		if (node->child != NULL)
			ast_print_recursive(node->child, depth + 1, 1);

		if (print_next == 0)
			return;

		node = node->next;
	}
}

void ast_print(ast *node) { ast_print_recursive(node, 0, 1); }
