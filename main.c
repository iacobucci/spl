#include "utils.h"
#include "parser.h"
#include "rule.h"
#include "json.h"
#include "ast.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
	json_init();
	// json_test();

	parse_result pr = parse("[\"ciao\", \"cane\", \"casa\"]", value);

	ast_print(root);


	printf("\n\n");

	return 0;
}
