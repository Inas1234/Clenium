#ifndef CHROMEDRIVER_H
#define CHROMEDRIVER_H

/**
 * Start a Chromedriver instance.
 * 
 * @param driver_path  Path to the chromedriver binary (e.g. "/usr/local/bin/chromedriver")
 * @param port         TCP port to listen on (e.g. 9515)
 * @return 0 on success (and Chromedriver is launching), -1 on error
 */
int chromedriver_start(const char *driver_path, int port);

/**
 * Stop the previously-started Chromedriver.
 * 
 * @return 0 on success, -1 if none running or on error
 */
int chromedriver_stop(void);

#endif // CHROMEDRIVER_H