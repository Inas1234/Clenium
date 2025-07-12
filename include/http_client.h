#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stddef.h>


int http_client_init(void);


void http_client_cleanup(void);


int http_client_get(const char *url, char **out_body, size_t *out_len);


int http_client_post(const char *url,
                     const char *body, size_t body_len,
                     char **out_body, size_t *out_len);

#endif // HTTP_CLIENT_H