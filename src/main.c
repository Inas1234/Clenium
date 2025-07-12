#include "http_client.h"
#include "json_parser.h"
#include "chromedriver.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    http_client_init();


    if (chromedriver_start("/usr/local/bin/chromedriver", 9515) != 0) {
        fprintf(stderr, "Failed to start Chromedriver\n");
        return 1;
    }

    char *body;
    size_t len;

    // Wait for Chromedriver to become ready, retrying up to 5 times
    int retries = 0;
    const int max_retries = 20;
    int status_ok = 0;
    while (retries < max_retries) {
        if (http_client_get("http://localhost:9515/status", &body, &len) == 0) {
            status_ok = 1;
            break;
        }
        sleep(1);
        retries++;
    }
    if (!status_ok) {
        fprintf(stderr, "Chromedriver /status failed after %d attempts\n", max_retries);
        return 1;
    }

    void *root = json_parse(body);
    if (!root) {
        fprintf(stderr, "JSON parse error\n");
        free(body);
        return 1;
    }

    char *version = json_get_string(root, "value.build.version");
    if (version) {
        printf("Chromedriver version: %s\n", version);
        free(version);
    } else {
        printf("Version key not found\n");
    }

    json_free(root);
    free(body);
    chromedriver_stop();
    http_client_cleanup();
    return 0;
}