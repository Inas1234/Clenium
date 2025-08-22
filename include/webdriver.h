#ifndef WEBDRIVER_H
#define WEBDRIVER_H

#include <stddef.h>

/** Opaque handle for a WebDriver session. */
typedef struct {
    char *session_id;
    char *endpoint;    // e.g. "http://localhost:9515"
} WDSession;

/** Opaque handle for a WebElement. */
typedef struct {
    char *element_id;
} WDElement;

/**
 * Create a new WebDriver session.
 * @param wd_url    Base URL of the driver (e.g. "http://localhost:9515")
 * @param session   Out parameter; session_id is allocated and must be freed.
 * @return 0 on success, non-zero on error.
 */
int wd_new_session(const char *wd_url, WDSession *session);

/**
 * Close an existing session.
 * @param session   Previously initialized session.
 * @return 0 on success, non-zero on error.
 */
int wd_quit_session(WDSession *session);

/**
 * Navigate the browser to the given URL.
 * @param session   Active session.
 * @param url       Target URL.
 * @return 0 on success, non-zero on error.
 */
int wd_navigate(WDSession *session, const char *url);

/**
 * Find the first element matching the locator.
 * @param session   Active session.
 * @param using     Locator strategy ("css selector", "xpath", etc.).
 * @param value     The selector string.
 * @param out_elem  Out parameter; element_id is allocated and must be freed.
 * @return 0 on success, non-zero on error.
 */
int wd_find_element(WDSession *session, const char *locator, const char *value, WDElement *out_elem);

/**
 * Click on an element.
 * @param session   Active session.
 * @param elem      Element handle.
 * @return 0 on success, non-zero on error.
 */
int wd_click(WDSession *session, WDElement *elem);

/**
 * Get the text content of an element.
 * @param session   Active session.
 * @param elem      Element handle.
 * @param out_text  Out parameter; allocated string, must be freed.
 * @return 0 on success, non-zero on error.
 */
int wd_get_text(WDSession *session, WDElement *elem, char **out_text);


/**
 * Send keystrokes to an element (e.g. input fields).
 * @param session   Active session.
 * @param elem      Element handle.
 * @param text      NUL-terminated string of characters to send.
 * @return 0 on success, non-zero on error.
 */
int wd_send_keys(WDSession *session, WDElement *elem, const char *text);



/**
 * Find all elements matching the locator.
 * @param session   Active session.
 * @param using     Locator strategy.
 * @param value     Selector string.
 * @param out_elems Allocated array of WDElement (caller must free each element_id and the array).
 * @param count     Number of elements found.
 * @return 0 on success, non-zero on error.
 */
int wd_find_elements(WDSession *session,
                     const char *locator,
                     const char *value,
                     WDElement **out_elems,
                     size_t *count);


/**
 * Take a screenshot of the current viewport.
 * @param session   Active session.
 * @param out_png   Base64-encoded PNG bytes (caller must free).
 * @return 0 on success, non-zero on error.
 */
int wd_take_screenshot(WDSession *session, char **out_png);



/**
 * Execute arbitrary JavaScript in the browser context.
 * @param session   Active session.
 * @param script    JS to run.
 * @param out_result Parsed JSON result (caller must json_free).
 */
int wd_execute_script(WDSession *session, const char *script, void **out_result);


WDElement* wd_wait_for_element(WDSession* sess, const char* locator, const char* value, int timeout_sec);


#endif // WEBDRIVER_H