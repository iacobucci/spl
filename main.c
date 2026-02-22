#include "ast.h"
#include "json.h"
#include "parser.h"
#include "rule.h"

#include <stdio.h>

void parse_tree(char *str, rule *r) {
	parse_result pr = parse(str, r);
	ast *root = pr.node;

	ast_print(root);
	printf("\n");

	ast_wipe(root, dq);
	ast_wipe(root, ws);
	ast_wipe(root, comma);
	ast_wipe(root, lsb);
	ast_wipe(root, rsb);

	ast_wipe(root, empty_array);

	ast_collapse(root, string);
	ast_collapse(root, number);
	ast_collapse(root, optional_minus);
	ast_collapse(root, non_decimal_part);
	ast_collapse(root, optional_decimal_part);
	ast_collapse(root, optional_exponential_part);

	ast_simplify(root, value_and_comma);
	ast_simplify(root, one_element_array);
	ast_simplify(root, many_elements_array);
	ast_simplify(root, one_or_more_values);
	ast_simplify(root, value);

	ast_print(root);
	printf("\n");

	// ast_print(root);

	printf("%s\n", (pr.matched == MATCHED) ? "matched" : "not matched");
}

int main() {
	parse_result pr;
	json_init();

	// parse_tree("[\"ciao\"]", value);
	parse_tree("[\"cane\", \"casa\", [\"cavallo catamarano\"], [], [] ]",
			   value);
	// parse_tree("256", value);
	// parse_tree("10.625", value);

	printf("value_and_comma %d\n", value_and_comma->id);
	printf("one_or_more_values %d\n", one_or_more_values->id);
	printf("many_elements_array %d\n", many_elements_array->id);

	json_free();
	return 0;
}
