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

// TODO: free
void ast_simplify_recursive(ast *node, int rid) {
	if (node == NULL)
		return;

	ast_simplify_recursive(node->child, rid);
	ast_simplify_recursive(node->next, rid);

	ast *parent = node->parent;

	if (node->ruleid == rid) {
		printf("\n\nrid = %d, prev = %p\n", rid, node->prev);

		if (parent != NULL) {
			ast *grandchild = node->child;

			if (node->prev == NULL) {
				printf("node->prev = NULL for %d\n", rid);

				parent->child = grandchild;
			} else {
				node->prev->next = grandchild;
				grandchild->prev = node->prev;
			}

			// ast *grandsibling = grandchild;
			//
			// while (grandsibling != NULL) {
			// 	grandsibling->parent = parent;
			// 	if (grandsibling->next == NULL)
			// 		break;
			// 	grandsibling = grandsibling->next;
			// }
			//
			// if (grandsibling != NULL) {
			// 	grandsibling->next = node->next;
			//
			// 	if (node->next)
			// 		node->next->prev = grandsibling;
			//
			// 	if (node->prev)
			// 		node->prev->next = grandsibling;
			//
			// 	ast *sibling = node->next;
			//
			// 	while (sibling != NULL) {
			// 		sibling->parent = parent;
			// 		sibling = sibling->next;
			// 	}
			// }
		}
	}

	if (node->ruleid == rid) {
		// node->next = NULL;
		// node->prev = NULL;
		// node->parent = NULL;
		// ast_free(&node);
	}
}

void ast_simplify(ast *node, rule *r) {
	ast_simplify_recursive(node, r->id);

	ast_print(node);

	if (r->method == ONE_OR_MORE)
		ast_simplify_recursive(node, r->childs[1]->id);
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

// int ast_is_subnode(ast *root, ast *other) {
// 	if (root == NULL || other == NULL)
// 		return 0;
//
// 	ast_is_subnode(root, other->child);
// 	ast_is_subnode(root, other->next);
// }

int collapsing_to_add_index;
int collapsing_to_add_buffer_length;

#define TO_ADD_INITIAL_BUFFER_LENGTH 4

void ast_collapse_recursive(ast *node, rule *r, ast *to_add, int collapsing) {
	if (node == NULL)
		return; // base case

	// iteratively add characters to the to_add->content
	if (collapsing && node->content) {
		to_add->content[collapsing_to_add_index] = node->content[0];
		// always add a terminator. May be inefficient.
		to_add->content[collapsing_to_add_index + 1] = '\0';

		collapsing_to_add_index++;

		// expand the size of the string
		if (collapsing_to_add_index == collapsing_to_add_buffer_length) {
			collapsing_to_add_buffer_length *= 2;
			to_add->content =
				realloc(to_add->content,
						sizeof(char) * (collapsing_to_add_buffer_length + 1));
		}
	}

	if (node->ruleid == r->id) {
		to_add = node;

		to_add->content =
			malloc(sizeof(char) * (TO_ADD_INITIAL_BUFFER_LENGTH + 1));

		collapsing_to_add_buffer_length = TO_ADD_INITIAL_BUFFER_LENGTH;

		collapsing_to_add_index = 0;
		collapsing = 1;
	}

	ast_collapse_recursive(node->child, r, to_add, collapsing);

	if (collapsing && collapsing_to_add_index == 0) {
		free(to_add->content);
		to_add->content = NULL;
	}

	if (node == to_add)
		collapsing = 0;

	ast_collapse_recursive(node->next, r, to_add, collapsing);

	if (node->ruleid == r->id) {
		if (node->child)
			ast_free(&(node->child));
	}
}

void ast_collapse(ast *node, rule *r) {
	ast_collapse_recursive(node, r, NULL, 0);
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
