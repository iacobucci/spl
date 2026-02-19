#include "ast.h"
#include "json.h"
#include "parser.h"
#include "rule.h"

#include <stdio.h>

void parse_tree(char *str, rule *r) {
	parse_result pr = parse(str, r);
	ast *root = pr.node;

	ast_print(root);

	// ast *sub = ast_collapse_only_childs(root);
	// ast_print(sub);

	printf("%s\n", (pr.matched == MATCHED) ? "matched" : "not matched");
}

int main() {
	parse_result pr;
	json_init();

	parse_tree("[\"ciao\"]", value);
	parse_tree("\"cane\"", value);
	parse_tree("256", value);

	// json_free();
	return 0;
}
