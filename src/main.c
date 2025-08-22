#include "chromedriver.h"
#include "webdriver.h"
#include "assertions.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// This is now a self-contained test case
void test_google_search_for_openai(WDSession* sess) {
    test_case("Google Search for OpenAI");

    wd_navigate(sess, "https://www.google.com");
    sleep(1);

    assert_element_found(sess, "css selector", "textarea[name='q']");

    WDElement search_box;
    if (wd_find_element(sess, "css selector", "textarea[name='q']", &search_box) == 0) {
        wd_send_keys(sess, &search_box, "OpenAI");
        sleep(1); 
    }

    WDElement search_button;
    if (wd_find_element(sess, "css selector", "input[name='btnK']", &search_button) == 0) {
        wd_click(sess, &search_button);
        free(search_button.element_id); // Free the element ID after use
    } 
    
    sleep(2);

    assert_element_found(sess, "css selector", "#search");
}

int main(void) {
    if (chromedriver_start("chromedriver", 9515) != 0) {
        fprintf(stderr, "Failed to start Chromedriver\n");
        return 1;
    }

    WDSession sess;
    if (wd_new_session("http://localhost:9515", &sess) != 0) {
        fprintf(stderr, "Failed to create session\n");
        chromedriver_stop();
        return 1;
    }

    // --- TEST EXECUTION ---
    test_suite_start("Google Search Suite");
    
    test_google_search_for_openai(&sess);
    
    test_suite_end();
    // --- END OF EXECUTION ---

    wd_quit_session(&sess);
    chromedriver_stop();

    return g_tests_failed > 0 ? 1 : 0;
}