#include "ast.h"
#include "json.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	// parse_result pr;
	// json_init();
	//
	// json_test();
	//
	// char *j = "{\"cane\": [\"a\", 1, -1, 0.34234, {\"empty\": [[]]}, "
	// 		  "-723.23, 2E10, "
	// 		  "-0.12e226, null], \"CAPRA\": {\"cavallo\": false} }";
	//
	// printf("%s\n\n", j);
	//
	// pr = json_parse(j);
	//
	// if (pr.matched == MATCHED) {
	// 	char *s = ast_to_string(pr.node);
	// 	printf("%s\n\n", s);
	// 	parse_result_free(pr);
	// 	pr = json_parse(s);
	// 	free(s);
	// 	if (pr.matched == MATCHED) {
	// 		char *s = ast_to_string(pr.node);
	// 		printf("%s\n", s);
	// 		free(s);
	// 		parse_result_free(pr);
	// 	}
	// }
	//
	// json_free();

	char *text = "123";
	rule *num = rule_one_or_more(rule_range('0', '9'));
	parse_result pr = parse(text, num);
	ast *a = pr.node;

	ast_collapse(a, num);

	if (pr.matched == MATCHED) {
		char *s = ast_to_string(a);
		printf("%s\n", s); // {"content":"123"}
		free(s);
	}

	rule_free(&num);
	parse_result_free(pr);

	return 0;
}
