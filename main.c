#include "ast.h"
#include "json.h"
#include "parser.h"
#include "rule.h"

#include <stdio.h>

void parse_tree_array(char *str, rule *r) {
	parse_result pr = parse(str, r);
	ast *root = pr.node;

	ast_print(root);
	printf("\n");

	ast_wipe(root, dq);
	ast_wipe(root, ws);
	ast_wipe(root, comma);
	ast_wipe(root, lsb);
	ast_wipe(root, rsb);

	ast_collapse(root, string);

	ast_print(root);

	// ast_simplify(root, many_elements_array);
	// ast_simplify(root, one_element_array);
	// ast_simplify(root, value_and_comma);
	ast_simplify(root, one_or_more_values);

	// ast_simplify(root, value);

	ast_print(root);

	printf("%s\n", (pr.matched == MATCHED) ? "matched" : "not matched");
}

void parse_tree_number(char *str, rule *r) {
	parse_result pr = parse(str, r);
	ast *root = pr.node;

	ast_print(root);
	printf("\n");

	ast_collapse(root, optional_minus);
	ast_collapse(root, non_decimal_part);
	ast_collapse(root, optional_decimal_part);
	ast_collapse(root, optional_exponential_part);

	// ast_collapse(root, number);

	// ast_simplify(root, value);

	ast_print(root);

	printf("%s\n", (pr.matched == MATCHED) ? "matched" : "not matched");
}

int main() {
	parse_result pr;
	json_init();

	// parse_tree_array("[\"ciao\"]", value);
	// parse_tree_array("[\"cane\", \"casa\", [\"cavallo catamarano\"] ]",
	// value);

	parse_tree_number("256", value);

	parse_tree_number("10.625", value);

	json_free();
	return 0;
}
