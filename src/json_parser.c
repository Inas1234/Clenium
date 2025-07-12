#include "json_parser.h"
#include <cjson/cJSON.h>
#include <stdlib.h>
#include <string.h>

void *json_parse(const char *text) {
    return (void*)cJSON_Parse(text);
}

char *json_get_string(void *root, const char *path) {
    if (!root || !path) return NULL;
    cJSON *item = (cJSON*)root;
    char *p = strdup(path);
    char *segment = strtok(p, ".");
    while (segment && item) {
        item = cJSON_GetObjectItemCaseSensitive(item, segment);
        segment = strtok(NULL, ".");
    }
    free(p);
    if (!item || !cJSON_IsString(item) || (item->valuestring == NULL)) {
        return NULL;
    }
    return strdup(item->valuestring);
}

void json_free(void *root) {
    if (root) {
        cJSON_Delete((cJSON*)root);
    }
}