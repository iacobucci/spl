#include "ast.h"
#include "json.h"
#include "parser.h"
#include "rule.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	parse_result pr;
	json_init();

	char *j =
		"{\"cane\": [\"a\", 1, -1, 0.34234, {\"empty\": [[]]}, -723.23, 2E10, "
		"-0.12e226, null], \"CAPRA\": {\"cavallo\": false} }";

	printf("%s\n\n", j);

	pr = json_parse(j);
	char *s = ast_to_string(pr.node);

	if (s != NULL) {
		printf("\n\n%s\n", s);
		parse_result pr = json_parse(s);
		printf("\nMatch: %s\n", pr.matched == MATCHED ? "YES" : "NO");
		if (pr.matched == NOT_MATCHED) {
			printf("Remaining: %s\n", pr.remaining);
		}
		char *s1 = ast_to_string(pr.node);
		if (s1 != NULL) {
			printf("\n\n%s\n", s1);
			free(s1);
		}
		free(s);
	}

	char *m =
		"{\"name\":\"value\",\"children\":[{\"name\":\"object\",\"children\":[{"
		"\"name\":\"member\",\"children\":[{\"name\":\"string\",\"content\":"
		"\"cane\"},{\"name\":\"array\",\"children\":[{\"name\":\"string\","
		"\"content\":\"a\"},{\"name\":\"number\",\"content\":\"1\"},{\"name\":"
		"\"number\",\"content\":\"-1\"},{\"name\":\"number\",\"content\":\"0."
		"34234\"},{\"name\":\"object\",\"children\":[{\"name\":\"member\","
		"\"children\":[{\"name\":\"string\",\"content\":\"empty\"},{\"name\":"
		"\"array\",\"children\":[{\"name\":\"array\"}]}]}]},{\"name\":"
		"\"number\",\"content\":\"-723.23\"},{\"name\":\"number\",\"content\":"
		"\"2E10\"},{\"name\":\"number\",\"content\":\"-0.12e226\"},{\"name\":"
		"\"null\",\"content\":\"null\"}]}]},{\"name\":\"member\",\"children\":["
		"{\"name\":\"string\",\"content\":\"CAPRA\"},{\"name\":\"object\","
		"\"children\":[{\"name\":\"member\",\"children\":[{\"name\":\"string\","
		"\"content\":\"cavallo\"},{\"name\":\"boolean\",\"content\":\"false\"}]"
		"}]}]}]}]}";

	pr = json_parse(m);

	printf("%s\n", (pr.matched == MATCHED) ? "YES" : "NO");

	json_free();

	return 0;
}
