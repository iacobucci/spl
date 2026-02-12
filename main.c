#include "ast.h"
#include "json.h"
#include "parser.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

int main() {
	json_init();
	// json_test();

	parse_result pr =
		parse("[\"ciao\", \"cane\", [ \"capra\" ], \"casa\"]", value);

	ast_print(pr.node);

	// printf("\n");
	// ast_print(root);

	// printf("\n");
	// ast_pop(root->child);
	// ast_print(root);

	printf("\n\n");

	if (fork()) {
		execl("/usr/bin/date", NULL);
	}

	return 0;
}
