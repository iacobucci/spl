#include <stdlib.h>

#include "../parser.h"
#include "../rule.h"

int main() {
	parse_assert_free("x", rule_literal("x"), MATCHED);
	parse_assert_free("x", rule_or(rule_literal("x"), rule_literal("y"), NULL),
				 MATCHED);
	parse_assert_free("y", rule_or(rule_literal("x"), rule_literal("y"), NULL),
				 MATCHED);
	parse_assert_free("xy", rule_and(rule_literal("x"), rule_literal("y"), NULL),
				 MATCHED);
	parse_assert_free("xy",
				 rule_and(rule_or(rule_literal("x"), rule_literal("y"), NULL),
						  rule_literal("y"), NULL),
				 MATCHED);
	parse_assert_free("yy",
				 rule_and(rule_or(rule_literal("x"), rule_literal("y"), NULL),
						  rule_literal("y"), NULL),
				 MATCHED);
	parse_assert_free("x",
				 rule_and(rule_or(rule_literal("x"), rule_literal("y"), NULL),
						  rule_optional(rule_literal("z")), NULL),
				 MATCHED);
	parse_assert_free("xz",
				 rule_and(rule_or(rule_literal("x"), rule_literal("y"), NULL),
						  rule_optional(rule_literal("z")), NULL),
				 MATCHED);
	parse_assert_free("yz",
				 rule_and(rule_or(rule_literal("x"), rule_literal("y"), NULL),
						  rule_optional(rule_literal("z")), NULL),
				 MATCHED);

	parse_assert_free(
		"x",
		rule_and(rule_literal("x"), rule_zero_or_more(rule_literal("y")), NULL),
		MATCHED);
	parse_assert_free(
		"xyyyyyyyyy",
		rule_and(rule_literal("x"), rule_zero_or_more(rule_literal("y")), NULL),
		MATCHED);
	parse_assert_free("xyyyyyyyyyz",
				 rule_and(rule_literal("x"),
						  rule_zero_or_more(rule_literal("y")),
						  rule_literal("z"), NULL),
				 MATCHED);
}
