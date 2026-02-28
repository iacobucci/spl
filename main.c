#include "ast.h"
#include "json.h"
#include "parser.h"

#include <stdio.h>

int main() {
	parse_result pr;
	json_init();

	json_test();

	char *j = "{\"cane\": [\"a\", 1, -1, 0.34234, {\"empty\": [[]]}, "
			  "-723.23, 2E10, "
			  "-0.12e226, null], \"CAPRA\": {\"cavallo\": false} }";

	printf("%s\n", j);

	pr = json_parse(j);

	if (pr.matched == MATCHED) {
		char *s = ast_to_string(pr.node);
		printf("%s\n", s);
	}
	parse_result_free(pr);

	json_free();
}
