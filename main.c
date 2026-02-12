#include "ast.h"
#include "json.h"
#include "parser.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

int main() {
	json_init();
	// json_test();

	parse_result pr = parse("[\"ciao\", \"cane\", [ 1 ], \"casa\"]", value);

	printf("\n");
	ast_print(root);

	printf("\n\n");

	return 0;
}
