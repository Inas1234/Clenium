#ifndef ASSERTIONS_H
#define ASSERTIONS_H

#include "webdriver.h"

extern int g_tests_failed;
extern int g_tests_passed;

void test_suite_start(const char* suite_name);

void test_suite_end(void);

void test_case(const char* test_name);

void assert_text_equals(WDSession* sess, WDElement* elem, const char* expected_text);
void assert_element_found(WDSession* sess, const char* locator, const char* value);
void assert_title_contains(WDSession* sess, const char* substring);

#endif 