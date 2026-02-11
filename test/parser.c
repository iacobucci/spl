#include <stdlib.h>

#include "../parser.h"
#include "../rule.h"

int main() {
	parse_assert("x", rule_literal("x"), MATCHED);
	parse_assert("x", rule_or(rule_literal("x"), rule_literal("y"), NULL),
				 MATCHED);
	parse_assert("y", rule_or(rule_literal("x"), rule_literal("y"), NULL),
				 MATCHED);
	parse_assert("xy", rule_and(rule_literal("x"), rule_literal("y"), NULL),
				 MATCHED);
	parse_assert("xy",
				 rule_and(rule_or(rule_literal("x"), rule_literal("y"), NULL),
						  rule_literal("y"), NULL),
				 MATCHED);
	parse_assert("yy",
				 rule_and(rule_or(rule_literal("x"), rule_literal("y"), NULL),
						  rule_literal("y"), NULL),
				 MATCHED);
	parse_assert("x",
				 rule_and(rule_or(rule_literal("x"), rule_literal("y"), NULL),
						  rule_optional(rule_literal("z")), NULL),
				 MATCHED);
	parse_assert("xz",
				 rule_and(rule_or(rule_literal("x"), rule_literal("y"), NULL),
						  rule_optional(rule_literal("z")), NULL),
				 MATCHED);
	parse_assert("yz",
				 rule_and(rule_or(rule_literal("x"), rule_literal("y"), NULL),
						  rule_optional(rule_literal("z")), NULL),
				 MATCHED);

	parse_assert(
		"x",
		rule_and(rule_literal("x"), rule_zero_or_more(rule_literal("y")), NULL),
		MATCHED);
	parse_assert(
		"xyyyyyyyyy",
		rule_and(rule_literal("x"), rule_zero_or_more(rule_literal("y")), NULL),
		MATCHED);
	parse_assert("xyyyyyyyyyz",
				 rule_and(rule_literal("x"),
						  rule_zero_or_more(rule_literal("y")),
						  rule_literal("z"), NULL),
				 MATCHED);
}
