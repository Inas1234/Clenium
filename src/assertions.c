#include "assertions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int g_tests_failed = 0;
int g_tests_passed = 0;

void test_suite_start(const char* suite_name) {
    printf("========================================\n");
    printf("RUNNING TEST SUITE: %s\n", suite_name);
    printf("========================================\n");
    g_tests_failed = 0;
    g_tests_passed = 0;
}

void test_suite_end(void) {
    printf("========================================\n");
    printf("TEST SUITE SUMMARY\n");
    printf("PASSED: %d, FAILED: %d\n", g_tests_passed, g_tests_failed);
    printf("========================================\n");
}

void test_case(const char* test_name) {
    printf("\n--- TEST: %s ---\n", test_name);
}

void _handle_pass(const char* message) {
    printf("[PASS] %s\n", message);
    g_tests_passed++;
}

void _handle_fail(const char* message) {
    printf("[FAIL] %s\n", message);
    g_tests_failed++;
}

void assert_text_equals(WDSession* sess, WDElement* elem, const char* expected_text) {
    char* actual_text = NULL;
    if (wd_get_text(sess, elem, &actual_text) != 0) {
        _handle_fail("Failed to get element text.");
        return;
    }

    if (strcmp(actual_text, expected_text) == 0) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Element text is '%s' as expected.", expected_text);
        _handle_pass(msg);
    } else {
        char msg[512];
        snprintf(msg, sizeof(msg), "Element text mismatch. Expected: '%s', Actual: '%s'", expected_text, actual_text);
        _handle_fail(msg);
    }
    free(actual_text);
}

void assert_element_found(WDSession* sess, const char* locator, const char* value) {
    WDElement elem;
    if (wd_find_element(sess, locator, value, &elem) == 0) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Element '%s' was found.", value);
        _handle_pass(msg);
        free(elem.element_id);
    } else {
        char msg[256];
        snprintf(msg, sizeof(msg), "Element '%s' was not found.", value);
        _handle_fail(msg);
    }
}
// You would also need to add wd_get_title to webdriver.c
// void assert_title_contains(WDSession* sess, const char* substring) { ... }