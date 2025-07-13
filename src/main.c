#include "chromedriver.h"
#include "http_client.h"
#include "json_parser.h"
#include "webdriver.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    // Initialize HTTP client and start Chromedriver
    http_client_init();
    if (chromedriver_start("chromedriver", 9515) != 0) {
        fprintf(stderr, "Failed to start Chromedriver\n");
        return 1;
    }

    // Create a new session
    WDSession sess;
    if (wd_new_session("http://localhost:9515", &sess) != 0) {
        fprintf(stderr, "Failed to create session\n");
        chromedriver_stop();
        http_client_cleanup();
        return 1;
    }

    wd_navigate(&sess, "https://www.google.com");
    sleep(2);

    WDElement search_box;
    if (wd_find_element(&sess, "css selector", "input[name='q'], textarea[name='q']", &search_box) != 0) {
        fprintf(stderr, "Search box not found\n");
    } else {
        printf("Search box found: %s\n", search_box.element_id);
        wd_click(&sess, &search_box);
        wd_send_keys(&sess, &search_box, "OpenAI");
    }
    sleep(2);   
    WDElement search_button;
    if (wd_find_element(&sess, "css selector", "input[name='btnK']", &search_button) == 0) {
        wd_click(&sess, &search_button);
    }
    else {
        fprintf(stderr, "Search button not found\n");
    }

    sleep(2);

    WDElement results;
    if (wd_find_element(&sess, "css selector", "#search", &results) == 0) {
        printf("Search results container found.\n");
    }

    WDElement *links;
    size_t link_count = 0;
    if (wd_find_elements(&sess, "css selector", "#search a", &links, &link_count) == 0) {
        printf("Found %zu links in results.\n", link_count);
        for (size_t i = 0; i < link_count; ++i) {
            free(links[i].element_id);
        }
        free(links);
    }

    // Take a screenshot
    char *png_base64 = NULL;
    if (wd_take_screenshot(&sess, &png_base64) == 0) {
        printf("Screenshot base64 length: %zu\n", strlen(png_base64));
        free(png_base64);
    }

    // Clean up
    wd_quit_session(&sess);
    chromedriver_stop();
    http_client_cleanup();
    return 0;
}