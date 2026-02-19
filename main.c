#include "ast.h"
#include "json.h"
#include "parser.h"
#include "rule.h"

#include <stdio.h>

int main() {
	parse_result pr;
	json_init();

	rule_print(value);
	// json_test();

	pr = parse("[\"ciao\"]", value);
	ast *root = pr.node;

	ast_print(root);

	ast *sub = ast_collapse_only_childs(root);
	ast_print(sub);

	// printf("%s\n", (pr.matched == MATCHED) ? "matched" : "not matched");

	json_free();
	rule_print(value);
	return 0;
}
