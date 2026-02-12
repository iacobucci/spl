#include "ast.h"
#include "json.h"
#include "parser.h"
#include "rule.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
	parse_result pr;
	json_init();
	// rule_print(value);

	// json_test();

	// pr = parse("[\"ciao\", \"cane\", [ \"capra\" ], \"casa\"]", value);
	pr = parse("1", value);

	// rule *r1 = rule_or(rule_zero_or_more(rule_c('x')), rule_c('y'), NULL);

	// rule *r1 = rule_or(rule_or(rule_c('x'), NULL), rule_or(rule_c('y'),
	// NULL), 				   rule_or(rule_c('z'), NULL), NULL);

	// rule *r1 =
	// 	rule_and(rule_c('a'), rule_zero_or_more(rule_c('b')),
	// 			 rule_or(rule_c('x'), rule_c('y'), rule_c('z'), NULL), NULL);
	//
	// pr = parse("abbbbbz", r1);
	//
	ast_print(ast_get_root(pr.node));

	if (fork()) {
		execl("/usr/bin/date", NULL);
	} else {
		wait(NULL);
		printf("\n");
	}

	return 0;
}
