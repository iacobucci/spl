#pragma once
#include "rule.h"

extern rule *lcb;
extern rule *rcb;

extern rule *lsb;
extern rule *rsb;

extern rule *space;
extern rule *tab;
extern rule *linefeed;
extern rule *cr;

extern rule *ws;

extern rule *non_zero_digit;

extern rule *zero;

extern rule *digit;

extern rule *zero_or_more_digits;

extern rule *one_or_more_digits;

extern rule *non_zero_starting_digits;

extern rule *non_decimal_part;

extern rule *minus;

extern rule *optional_minus;

extern rule *point;

extern rule *decimal_part;

extern rule *optional_decimal_part;

extern rule *exponent;

extern rule *sign;

extern rule *optional_sign;

extern rule *exponential_part;

extern rule *optional_exponential_part;

extern rule *number;

extern rule *dq;

extern rule *character;

extern rule *characters;

extern rule *hex;

extern rule *backslash;

extern rule *escaped_unicode;
extern rule *escaped_quotation_mark;
extern rule *escaped_backslash;
extern rule *escaped_forewards_slash;
extern rule *escaped_backspace;
extern rule *escaped_formfeed;
extern rule *escaped_linefeed;

extern rule *escaped_cr;

extern rule *escaped_tab;

extern rule *escape;

extern rule *zero_or_more_optional_escapes_or_characters;

extern rule *string;

extern rule *double_dots;

extern rule *comma;

extern rule *value;

extern rule *empty_array;

extern rule *one_element_array;

extern rule *value_and_comma;

extern rule *one_or_more_values;

extern rule *many_elements_array;

extern rule *array;

extern rule *member;

extern rule *member_and_comma;

extern rule *one_or_more_members;

extern rule *empty_object;

extern rule *one_member_object;

extern rule *many_elements_object;

extern rule *object;

extern rule *boolean_true;
extern rule *boolean_false;

extern rule *boolean;

extern rule *null;

void json_init();

void json_free();

void json_test();
