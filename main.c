#include "ast.h"
#include "json.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	parse_result pr;
	json_init();

	json_test();

	char *j = "{\"cane\": [\"a\", 1, -1, 0.34234, {\"empty\": [[]]}, "
			  "-723.23, 2E10, "
			  "-0.12e226, null], \"CAPRA\": {\"cavallo\": false} }";

	printf("%s\n\n", j);

	pr = json_parse(j);

	char *s = ast_to_string(pr.node);

	if (s != NULL) {
		printf("%s\n\n", s);
		parse_result pr1 = json_parse(s);
		char *s1 = ast_to_string(pr1.node);
		if (s1 != NULL) {
			printf("%s\n", s1);
			free(s1);
		}
		free(s);
	}

	json_free();

	return 0;
}
