#include <cjson/cJSON.h>
#include "webdriver.h"
#include "http_client.h"
#include "json_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static void *post_json(WDSession *sess, 
                       const char *path, 
                       const char *json_body) 
{
    char url[512];
    snprintf(url, sizeof(url), "%s%s", sess->endpoint, path);

    char *resp; size_t len;
    if (http_client_post(url, json_body, strlen(json_body), &resp, &len) != 0) {
        return NULL;
    }
    void *root = json_parse(resp);
    free(resp);
    return root;
}

// Temporarily modify for debugging
int wd_new_session(const char *wd_url, WDSession *session) {
    session->endpoint = strdup(wd_url);

    cJSON *root = cJSON_CreateObject();
    cJSON *caps = cJSON_AddObjectToObject(root, "capabilities");
    cJSON *alwaysMatch = cJSON_AddObjectToObject(caps, "alwaysMatch");
    cJSON *googChromeOptions = cJSON_AddObjectToObject(alwaysMatch, "goog:chromeOptions");

    cJSON *args = cJSON_CreateArray();
    cJSON_AddItemToArray(args, cJSON_CreateString("--disable-blink-features=AutomationControlled"));
    cJSON_AddItemToArray(args, cJSON_CreateString("--user-agent=Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/108.0.0.0 Safari/537.36"));
    cJSON_AddItemToObject(googChromeOptions, "args", args);

    cJSON *excludeSwitches = cJSON_CreateArray();
    cJSON_AddItemToArray(excludeSwitches, cJSON_CreateString("enable-automation"));
    cJSON_AddObjectToObject(googChromeOptions, "excludeSwitches");
    cJSON_AddItemToObject(googChromeOptions, "excludeSwitches", excludeSwitches);

    char *json_body = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    void *resp_root = post_json(session, "/session", json_body);
    free(json_body);

    if (!resp_root) return -1;


    char *sid = json_get_string(resp_root, "value.sessionId");
    if (!sid) sid = json_get_string(resp_root, "sessionId"); 
    json_free(resp_root);
    if (!sid) return -1;

    session->session_id = sid;
    return 0;
}

int wd_quit_session(WDSession *session) {
    char path[256];
    snprintf(path, sizeof(path), "/session/%s", session->session_id);
    void *root = post_json(session, path, ""); 
    json_free(root);
    free(session->session_id);
    free(session->endpoint);
    return 0;
}

int wd_navigate(WDSession *session, const char *url) {
    char path[256];
    snprintf(path, sizeof(path), "/session/%s/url", session->session_id);
    char body[512];
    snprintf(body, sizeof(body), "{\"url\":\"%s\"}", url);
    void *root = post_json(session, path, body);
    if (!root) return -1;
    json_free(root);
    return 0;
}

int wd_find_element(WDSession *session,
                    const char *locator,
                    const char *value,
                    WDElement *out_elem)
{
    char path[256];
    snprintf(path, sizeof(path), "/session/%s/element", session->session_id);
    char body[512];
    snprintf(body, sizeof(body),
             "{\"using\":\"%s\",\"value\":\"%s\"}", locator, value);
    void *root = post_json(session, path, body);
    if (!root) return -1;

    char *eid = json_get_string(root, "value.ELEMENT");
    if (!eid) eid = json_get_string(root, "value.element-6066-11e4-a52e-4f735466cecf"); 
    json_free(root);
    if (!eid) return -1;

    out_elem->element_id = eid;
    return 0;
}

int wd_click(WDSession *session, WDElement *elem) {
    char path[256];
    snprintf(path, sizeof(path),
             "/session/%s/element/%s/click",
             session->session_id, elem->element_id);
    void *root = post_json(session, path, "{}");
    json_free(root);
    return 0;
}

int wd_get_text(WDSession *session, WDElement *elem, char **out_text) {
    char url[512];
    snprintf(url, sizeof(url),
             "%s/session/%s/element/%s/text",
             session->endpoint, session->session_id, elem->element_id);

    char *resp = NULL;
    size_t len = 0;
    // Use GET for the text endpoint
    if (http_client_get(url, &resp, &len) != 0) {
        return -1;
    }

    void *root = json_parse(resp);
    free(resp);
    if (!root) {
        return -1;
    }

    char *text = json_get_string(root, "value");
    json_free(root);
    if (!text) {
        return -1;
    }

    *out_text = text;
    return 0;
}

int wd_send_keys(WDSession *session, WDElement *elem, const char *text) {
    char path[512];
    snprintf(path, sizeof(path),
             "/session/%s/element/%s/value",
             session->session_id, elem->element_id);

    // Build the JSON payload according to the W3C WebDriver standard.
    cJSON *root = cJSON_CreateObject();
    if (cJSON_AddStringToObject(root, "text", text) == NULL) {
        cJSON_Delete(root);
        return -1; // Failed to create JSON
    }

    char *body = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    if (body == NULL) {
        fprintf(stderr, "Failed to print JSON for send_keys\n");
        return -1;
    }

    // Send the request
    void *resp_root = post_json(session, path, body);
    free(body);

    if (!resp_root) {
        return -1;
    }

    json_free(resp_root);
    return 0;
}

int wd_find_elements(WDSession *session,
                     const char *using,
                     const char *value,
                     WDElement **out_elems,
                     size_t *count) {
    char path[512];
    snprintf(path, sizeof(path),
             "%s/session/%s/elements",
             session->endpoint, session->session_id);
    char body[512];
    snprintf(body, sizeof(body),
             "{\"using\":\"%s\",\"value\":\"%s\"}",
             using, value);

    char *resp = NULL;
    size_t len = 0;
    if (http_client_post(path, body, strlen(body), &resp, &len) != 0) {
        return -1;
    }
    cJSON *root = cJSON_Parse(resp);
    free(resp);
    if (!root) return -1;

    cJSON *arr = cJSON_GetObjectItemCaseSensitive(root, "value");
    if (!cJSON_IsArray(arr)) {
        cJSON_Delete(root);
        return -1;
    }
    size_t n = cJSON_GetArraySize(arr);
    *count = n;
    *out_elems = calloc(n, sizeof(WDElement));
    for (size_t i = 0; i < n; ++i) {
        cJSON *item = cJSON_GetArrayItem(arr, i);
        cJSON *eid = cJSON_GetObjectItemCaseSensitive(item, "ELEMENT");
        if (!cJSON_IsString(eid)) {
            eid = cJSON_GetObjectItemCaseSensitive(item, "element-6066-11e4-a52e-4f735466cecf");
        }
        (*out_elems)[i].element_id = strdup(eid->valuestring);
    }
    cJSON_Delete(root);
    return 0;
}

int wd_take_screenshot(WDSession *session, char **out_png) {
    char url[512];
    snprintf(url, sizeof(url),
             "%s/session/%s/screenshot",
             session->endpoint, session->session_id);

    char *resp = NULL;
    size_t len = 0;
    if (http_client_get(url, &resp, &len) != 0) {
        return -1;
    }
    void *root = json_parse(resp);
    free(resp);
    if (!root) {
        return -1;
    }
    char *png = json_get_string(root, "value");
    json_free(root);
    if (!png) {
        return -1;
    }
    *out_png = png;
    return 0;
}

WDElement* wd_wait_for_element(WDSession* sess, const char* locator, const char* value, int timeout_sec) {
    time_t start_time = time(NULL);
    while ((time(NULL) - start_time) < timeout_sec) {
        WDElement* elem = malloc(sizeof(WDElement));
        if (wd_find_element(sess, locator, value, elem) == 0) {
            return elem; // Found it!
        }
        free(elem);
        usleep(500 * 1000); // Wait 500ms before trying again
    }
    fprintf(stderr, "Timeout: Element '%s' not found after %d seconds.\n", value, timeout_sec);
    return NULL; // Timeout reached
}
