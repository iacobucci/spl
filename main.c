#include "ast.h"
#include "json.h"
#include "parser.h"
#include "rule.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_tree(char *str) {
	parse_result pr = parse(str, value);
	ast *root = pr.node;

	ast_print(root);
	printf("\n");

	ast_wipe(root, dq);
	ast_wipe(root, ws);
	ast_wipe(root, comma);
	ast_wipe(root, lsb);
	ast_wipe(root, rsb);
	ast_wipe(root, lcb);
	ast_wipe(root, rcb);
	ast_wipe(root, double_dots);

	ast_wipe(root, empty_array);
	ast_wipe(root, empty_object);

	ast_collapse(root, null);
	ast_collapse(root, boolean);
	ast_collapse(root, string);
	ast_collapse(root, number);

	ast_simplify(root, value_and_comma);
	ast_simplify(root, one_element_array);
	ast_simplify(root, many_elements_array);
	ast_simplify(root, one_or_more_values);

	ast_simplify(root, member_and_comma);
	ast_simplify(root, one_or_more_members);
	ast_simplify(root, one_member_object);
	ast_simplify(root, many_elements_object);

	ast_simplify(root, value);

	ast_print(root);

	printf("%s\n", (pr.matched == MATCHED) ? "matched" : "not matched");

	printf("\n");
}

int main() {
	parse_result pr;
	json_init();

	char *j = "{\"cane\": [1, -1, 0.34234, {\"empty\": [[]]}, -723.23, 2E10, "
			  "-0.12e226, null], \"CAPRA\": {\"cavallo\"  :false} }";

	parse_tree(j);

	json_free();

	char *s = malloc(sizeof(char) * 100);
	int len = sprintf(s, "ciao %s!!", "cane");

	char *s2 = malloc(sizeof(char) * 100);
	len = sprintf(s2, "the string was %s", s);
	free(s);

	printf("%s: %d\n", s2, len);
	free(s2);

	char *a = "ciao";
	char *b = "cane";

	char *c = string_concat(a, b);

	printf("%s\n", c);

	free(c);

	return 0;
}
