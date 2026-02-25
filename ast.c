#include "ast.h"
#include "rule.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TO_ADD_INITIAL_BUFFER_LENGTH 4

ast *ast_new(char *content, char *name) {
	ast *result = malloc(sizeof(ast));

	error_check_malloc(result);

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

void ast_simplify_recursive(ast *node, int rid) {
	if (node == NULL)
		return;

	ast_simplify_recursive(node->child, rid);
	ast_simplify_recursive(node->next, rid);

	if (node->ruleid != rid)
		return;
	if (node->child == NULL) {
		if (node->next)
			node->next->prev = node->prev;

		if (node->prev)
			node->prev->next = node->next;

		if (node->content)
			free(node->content);
		if (node->name)
			free(node->name);

		free(node);
		return;
	}

	ast *parent = node->parent;
	ast *prev = node->prev;
	ast *next = node->next;

	ast *first_child = node->child;
	ast *last_child = first_child;
	while (last_child->next != NULL)
		last_child = last_child->next;

	for (ast *c = first_child; c != NULL; c = c->next)
		c->parent = parent;

	if (prev == NULL) {
		if (parent != NULL)
			parent->child = first_child;
		first_child->prev = NULL;
	} else {
		prev->next = first_child;
		first_child->prev = prev;
	}

	last_child->next = next;
	if (next != NULL)
		next->prev = last_child;

	if (node->ruleid == rid && node->child != NULL && node->parent != NULL) {
		node->child = NULL;
		node->next = NULL;
		node->prev = NULL;
		if (node->content)
			free(node->content);
		if (node->name)
			free(node->name);
		free(node);
	}
}

void ast_simplify(ast *node, rule *r) {
	ast_simplify_recursive(node, r->id);

	if (r->method == ONE_OR_MORE) {
		ast_simplify_recursive(node, r->childs[1]->id);
	}
}

void ast_wipe(ast *node, rule *r) {
	if (node == NULL)
		return;

	ast *next = node->next;

	if (node->ruleid == r->id) {
		if (node->prev != NULL)
			node->prev->next = node->next;
		else if (node->parent != NULL)
			node->parent->child = node->next;

		if (node->next != NULL)
			node->next->prev = node->prev;

		ast_free_individual_node(node);
	} else {
		ast_wipe(node->child, r);
	}

	ast_wipe(next, r);
}

typedef struct {
	ast *target;
	char *buffer;
	int index;
	int capacity;
} collapse_state;

static void state_append(collapse_state *state, char c) {
	if (state->index == state->capacity) {
		state->capacity *= 2;

		state->buffer = realloc(state->buffer, state->capacity + 1);
		error_check_malloc(state->buffer);

		if (state->buffer == NULL)
			exit(-1);
	}
	state->buffer[state->index++] = c;
	state->buffer[state->index] = '\0';
}

static void ast_collapse_recursive(ast *node, rule *r, collapse_state *state) {
	if (node == NULL)
		return;

	int created_state = 0;
	collapse_state local_state = {0};

	if (node->ruleid == r->id) {
		local_state.target = node;
		local_state.capacity = TO_ADD_INITIAL_BUFFER_LENGTH;

		local_state.buffer = malloc(local_state.capacity + 1);
		error_check_malloc(local_state.buffer);

		local_state.buffer[0] = '\0';
		local_state.index = 0;
		state = &local_state;
		created_state = 1;
	}

	if (state != NULL && state->target != node && node->content) {
		state_append(state, node->content[0]);
	}

	ast_collapse_recursive(node->child, r, state);

	ast_collapse_recursive(node->next, r, created_state ? NULL : state);

	if (created_state) {
		if (local_state.index > 0) {
			if (node->content)
				free(node->content);
			node->content = local_state.buffer;
		} else {
			free(local_state.buffer);
			node->content = NULL;
		}

		if (node->child)
			ast_free(&(node->child));
		node->child = NULL;
	}
}

void ast_collapse(ast *node, rule *r) { ast_collapse_recursive(node, r, NULL); }

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

		if (node->name == NULL)
			printf("{");
		else
			printf("{\"name\": \"%s\"", node->name);

		if (node->name && node->content)
			printf(", ");

		if (node->content != NULL)
			printf("\"content\": \"%s\"", node->content);

		if (node->child != NULL) {
			printf(", \"children\": [\n");
			ast_print_recursive(node->child, depth + 1, 1);

			tabs_print(depth);
			printf("]}");
		} else {
			printf("}");
		}

		if (node->next) {
			printf(",");
		}

		printf("\n");

		if (print_next == 0)
			return;

		node = node->next;
	}
}

void ast_print(ast *node) { ast_print_recursive(node, 0, 1); }

void ast_to_string_recursive(ast *node, int depth, int print_next, char **buf) {
	while (node != NULL) {
		// char *tabs = tabs_to_string(depth);
		// *buf = string_concat(*buf, tabs);
		// free(tabs);

		if (node->name == NULL)
			*buf = string_concat(*buf, "{");
		else {
			int needed = snprintf(NULL, 0, "{\"name\": \"%s\"", node->name) + 1;
			char *local = malloc(needed * sizeof(char));
			sprintf(local, "{\"name\": \"%s\"", node->name);
			*buf = string_concat(*buf, local);
			free(local);
		}

		// if (node->name && node->content)
		// 	*buf = string_concat(*buf, ",");

		if (node->content != NULL) {
			int needed =
				snprintf(NULL, 0, "\"content\": \"%s\"", node->content) + 1;
			char *local = malloc(needed * sizeof(char));
			sprintf(local, "\"content\": \"%s\"", node->content);
			string_concat(*buf, local);
			free(local);
		}

		if (node->child != NULL) {
			*buf = string_concat(*buf, ", \"children\": [");

			ast_to_string_recursive(node->child, depth + 1, 1, buf);

			// char *tabs = tabs_to_string(depth);
			// *buf = string_concat(*buf, tabs);
			*buf = string_concat(*buf, "]}");

			// free(tabs);
		} else {
			*buf = string_concat(*buf, "}");
		}

		if (node->next) {
			*buf = string_concat(*buf, ",");
		}

		if (print_next == 0)
			return;

		node = node->next;
	}
}

char *ast_to_string(ast *node) {
	char *result;
	ast_to_string_recursive(node, 0, 1, &result);
	return result;
}
