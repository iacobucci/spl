#include "ast.h"
#include "json.h"
#include "parser.h"
#include "rule.h"
#include "utils.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	json_init();
	// json_test();

	parse_result pr = parse("[\"ciao\", \"cane\", \"casa\"]", value);

	printf("\n");
	ast_print(root);

	printf("\n\n");

	return 0;
}
