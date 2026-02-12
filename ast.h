#pragma once

typedef struct ast {
	struct ast *parent;
	struct ast *child;
	struct ast *next;
	struct ast *prev;
	int n_childs;
	char *content;
} ast;

ast *ast_new(char *content);

ast *ast_add_child(ast *parent, char *content);

ast *ast_pop(ast *node);

void ast_free(ast **np);

void ast_print(ast *node);