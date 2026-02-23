#include "ast.h"
#include "json.h"
#include "parser.h"
#include "rule.h"

#include <stdio.h>

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

	// parse_tree("[\"ciao\"]");
	// parse_tree("[\"cane\", \"casa\", [\"cavallo catamarano\"], [], [] ]");
	// parse_tree("256");
	// parse_tree("10.625");
	parse_tree("{\"cane\": [1, -1, 0.34234, {\"empty\": [[]]}, -723.23, 2E10, "
			   "-0.12e226, null], \"CAPRA\": {\"cavallo\"  :false} }");

	json_free();
	return 0;
}
