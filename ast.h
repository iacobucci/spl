#pragma once

typedef struct ast {
	struct ast *parent;
	struct ast *child;
	struct ast *next;
	struct ast *prev;
	int n_childs;
	char *content;

	void (*callback)(struct ast *self);
} ast;

ast *ast_new(char *content);

ast *ast_add_child(ast *parent, ast *child);

ast *ast_add_sibling(ast *parent, ast *sibling);

ast *ast_collapse_only_childs(ast *node);

ast *ast_pop(ast *node);

ast *ast_get_root(ast *node);

void ast_free(ast **np);

void ast_print(ast *node);
