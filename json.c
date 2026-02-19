#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "json.h"
#include "parser.h"
#include "rule.h"

rule *lcb;
rule *rcb;

rule *lsb;
rule *rsb;

rule *space;
rule *tab;
rule *linefeed;
rule *cr;

rule *ws;

rule *non_zero_digit;

rule *zero;

rule *digit;

rule *zero_or_more_digits;

rule *one_or_more_digits;

rule *non_zero_starting_digits;

rule *non_decimal_part;

rule *minus;

rule *optional_minus;

rule *point;

rule *decimal_part;

rule *optional_decimal_part;

rule *exponent;

rule *sign;

rule *optional_sign;

rule *exponential_part;

rule *optional_exponential_part;

rule *number;

rule *dq;

rule *character;

rule *characters;

rule *hex;

rule *backslash;

rule *escaped_unicode;
rule *escaped_quotation_mark;
rule *escaped_backslash;
rule *escaped_forward_slash;
rule *escaped_backspace;
rule *escaped_formfeed;
rule *escaped_linefeed;

rule *escaped_cr;

rule *escaped_tab;

rule *escape;

rule *zero_or_more_optional_escapes_or_characters;

rule *string;

rule *double_dots;

rule *comma;

rule *value;

rule *empty_array;

rule *one_element_array;

rule *value_and_comma;

rule *one_or_more_values;

rule *many_elements_array;

rule *array;

rule *member;

rule *member_and_comma;

rule *one_or_more_members;

rule *empty_object;

rule *one_member_object;

rule *many_elements_object;

rule *object;

rule *boolean_true;
rule *boolean_false;

rule *boolean;

rule *null;

void json_init() {
	lcb = rule_c('{');
	rcb = rule_c('}');

	lsb = rule_c('[');
	rsb = rule_c(']');

	space = rule_c(' ');
	tab = rule_c('\t');
	linefeed = rule_c('\n');
	cr = rule_c('\r');

	ws = rule_zero_or_more(rule_or(space, tab, linefeed, cr, NULL));

	non_zero_digit = rule_range('1', '9');

	zero = rule_c('0');

	digit = rule_or(non_zero_digit, zero, NULL);

	zero_or_more_digits = rule_zero_or_more(digit);

	one_or_more_digits = rule_one_or_more(digit);

	non_zero_starting_digits =
		rule_and(non_zero_digit, zero_or_more_digits, NULL);

	non_decimal_part = rule_or(zero, non_zero_starting_digits, NULL);

	minus = rule_c('-');

	optional_minus = rule_optional(minus);

	point = rule_c('.');

	decimal_part = rule_and(point, one_or_more_digits, NULL);

	optional_decimal_part = rule_optional(decimal_part);

	exponent = rule_or(rule_c('e'), rule_c('E'), NULL);

	sign = rule_or(minus, rule_c('+'), NULL);

	optional_sign = rule_optional(sign);

	exponential_part =
		rule_and(exponent, optional_sign, one_or_more_digits, NULL);

	optional_exponential_part = rule_optional(exponential_part);

	number = rule_add_name(rule_and(optional_minus, non_decimal_part,
									optional_decimal_part,
									optional_exponential_part, NULL),
						   "number");

	dq = rule_c('"');

	character =
		rule_or(digit, rule_range('a', 'z'), rule_range('A', 'Z'), space, NULL);

	characters = rule_zero_or_more(character);

	hex = rule_or(rule_range('a', 'f'), rule_range('A', 'F'), digit, NULL);

	backslash = rule_c('\\');

	escaped_unicode =
		rule_and(backslash, rule_c('u'), hex, hex, hex, hex, NULL);

	escaped_quotation_mark = rule_and(backslash, dq, NULL);

	escaped_backslash = rule_and(backslash, backslash, NULL);

	escaped_forward_slash = rule_and(backslash, rule_c('/'), NULL);

	escaped_backspace = rule_and(backslash, rule_c('b'), NULL);

	escaped_formfeed = rule_and(backslash, rule_c('f'), NULL);

	escaped_linefeed = rule_and(backslash, rule_c('n'), NULL);

	escaped_cr = rule_and(backslash, rule_c('r'), NULL);

	escaped_tab = rule_and(backslash, rule_c('t'), NULL);

	escape = rule_or(escaped_unicode, escaped_backslash, escaped_forward_slash,
					 escaped_backspace, escaped_formfeed, escaped_linefeed,
					 escaped_cr, escaped_tab, escaped_quotation_mark, NULL);

	zero_or_more_optional_escapes_or_characters =
		rule_zero_or_more(rule_or(character, escape, NULL));

	string = rule_add_name(
		rule_and(dq, zero_or_more_optional_escapes_or_characters, dq, NULL),
		"string");

	double_dots = rule_c(':');

	comma = rule_c(',');

	value = malloc(sizeof(rule));

	empty_array = rule_and(lsb, ws, rsb, NULL);

	one_element_array = rule_and(lsb, ws, value, ws, rsb, NULL);

	value_and_comma = rule_and(value, ws, comma, ws, NULL);

	one_or_more_values = rule_one_or_more(value_and_comma);

	many_elements_array =
		rule_and(lsb, ws, one_or_more_values, ws, value, ws, rsb, NULL);

	array = rule_or(many_elements_array, one_element_array, empty_array, NULL);

	member = rule_and(ws, string, ws, double_dots, ws, value, NULL);

	member_and_comma = rule_and(member, ws, comma, ws, NULL);

	one_or_more_members = rule_one_or_more(member_and_comma);

	empty_object = rule_and(lcb, ws, rcb, NULL);

	one_member_object = rule_and(lcb, ws, member, ws, rcb, NULL);

	many_elements_object =
		rule_and(lcb, ws, one_or_more_members, ws, member, ws, rcb, NULL);

	object =
		rule_or(empty_object, one_member_object, many_elements_object, NULL);

	boolean_true = rule_literal("true");
	boolean_false = rule_literal("false");

	boolean = rule_or(boolean_false, boolean_true, NULL);

	null = rule_literal("null");

	value->method = OR;
	value->n_childs = 6;
	value->childs = malloc(sizeof(rule *) * value->n_childs);
	value->childs[0] = number;
	value->childs[1] = string;
	value->childs[2] = boolean;
	value->childs[3] = null;
	value->childs[4] = array;
	value->childs[5] = object;
}

void json_free() { rule_free(&value); }

void json_test() {
	parse_assert(" \t\t  ", ws, MATCHED);
	parse_assert("1", non_zero_digit, MATCHED);
	parse_assert("0", digit, MATCHED);
	parse_assert("012300", one_or_more_digits, MATCHED);
	parse_assert("12300", non_zero_starting_digits, MATCHED);
	parse_assert(".0001", decimal_part, MATCHED);
	parse_assert("e-12", exponential_part, MATCHED);
	parse_assert("-1230.445e-8", number, MATCHED);
	parse_assert("0.1E-8", number, MATCHED);
	parse_assert("abC4d", characters, MATCHED);
	parse_assert("\\u1ab0", escaped_unicode, MATCHED);
	parse_assert("\"\\u1ab0 \\\"ciao\\\"\"", string, MATCHED);
	parse_assert("{}", value, MATCHED);
	parse_assert("[1,2,3,[4,5,6]]", value, MATCHED);
	parse_assert("{\"ciao\":10}", value, MATCHED);
	parse_assert("{\"ciao\":{\"cane\":10}}", value, MATCHED);
	parse_assert("{\"cane\": [1, -1, 0.34234, -723.23, 2E10, -0.12e226]}",
				 value, MATCHED);
	parse_assert("{\"cane\": [1, -1, 0.34234, -723.23, 2E10, "
				 "-0.12e226, null], \"CAPRA\": {\"cavallo\"  :false} }",
				 value, MATCHED);
}
