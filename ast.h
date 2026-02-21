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
	char *name;
	char *content;

	void (*callback)(struct ast *self);
} ast;

#endif

ast *ast_new(char *content, char* name);

ast *ast_add_child(ast *parent, ast *child);

ast *ast_add_sibling(ast *parent, ast *sibling);

ast *ast_collapse_only_childs(ast *node);

ast *ast_simplify(ast *node, struct rule* rule);

ast *ast_pop(ast *node);

ast *ast_get_root(ast *node);

void ast_free(ast **np);

void ast_print(ast *node);
