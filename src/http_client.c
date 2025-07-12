#include "http_client.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

struct memory {
    char *data;
    size_t size;
};

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    struct memory *mem = (struct memory*)userp;

    char *tmp = realloc(mem->data, mem->size + real_size + 1);
    if (!tmp) return 0;  // out of memory
    mem->data = tmp;
    memcpy(&(mem->data[mem->size]), ptr, real_size);
    mem->size += real_size;
    mem->data[mem->size] = '\0';
    return real_size;
}

int http_client_init(void) {
    return (curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK) ? 0 : -1;
}

void http_client_cleanup(void) {
    curl_global_cleanup();
}

static int perform_request(CURL *curl, char **out_body, size_t *out_len) {
    struct memory chunk = { .data = malloc(1), .size = 0 };
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        free(chunk.data);
        return -1;
    }

    *out_body = chunk.data;
    *out_len  = chunk.size;
    return 0;
}

int http_client_get(const char *url, char **out_body, size_t *out_len) {
    CURL *curl = curl_easy_init();
    if (!curl) return -1;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    int ret = perform_request(curl, out_body, out_len);
    curl_easy_cleanup(curl);
    return ret;
}

int http_client_post(const char *url,
                     const char *body, size_t body_len,
                     char **out_body, size_t *out_len) {
    CURL *curl = curl_easy_init();
    if (!curl) return -1;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)body_len);
    int ret = perform_request(curl, out_body, out_len);
    curl_easy_cleanup(curl);
    return ret;
}