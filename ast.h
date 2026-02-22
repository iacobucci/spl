#pragma once

#ifndef STRUCT_AST
#define STRUCT_AST

struct rule;

typedef struct ast {
	struct ast *parent;
	struct ast *child;
	struct ast *next;
	struct ast *prev;
	int n_childs;

	int ruleid;
	// char c; // TODO: move to char

	char *name;
	char *content;

	void (*callback)(struct ast *self);
} ast;

#endif

ast *ast_new(char *content, char* name);

ast *ast_add_child(ast *parent, ast *child);

ast *ast_add_sibling(ast *parent, ast *sibling);

ast *ast_pop(ast *node);

ast *ast_get_root(ast *node);

void ast_free(ast **np);

void ast_print(ast *node);

void ast_simplify(ast *node, struct rule* rule);

void ast_wipe(ast *node, struct rule* rule);

void ast_collapse(ast *node, struct rule* rule);

