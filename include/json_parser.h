#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <stddef.h>


void *json_parse(const char *text);


char *json_get_string(void *root, const char *path);


void json_free(void *root);

#endif // JSON_PARSER_H