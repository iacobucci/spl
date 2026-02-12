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

	ast *ciao = ast_new("ciao");
	ast *cane = ast_add_child(ciao, "cane");
	ast *casa = ast_add_child(ciao, "casa");
	ast *capra = ast_add_child(casa, "capra");
	ast *cavallo = ast_add_child(ciao, "cavallo");

	// ast_print(ciao);
	// ast_free(&ciao);

	json_init();
	// json_test();

	parse_result pr = parse("[\"ciao\", \"cane\", \"casa\"]", value);

	// printf("\n");

	// pr = parse("[[[\"xyz\"],\"123\"], \"abc\"]", value);

	printf("\n\n");

	return 0;
}
