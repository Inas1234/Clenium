#include "chromedriver.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>


static pid_t cd_pid = -1;

int chromedriver_start(const char *driver_path, int port) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) { 
        
        char port_arg[32];
        snprintf(port_arg, sizeof(port_arg), "--port=%d", port);

        execlp(driver_path, driver_path, port_arg, (char*)NULL);
        
        perror("execlp"); 
        exit(1);
    }

    cd_pid = pid;
    printf("Started Chromedriver with PID %d\n", cd_pid);
    
    sleep(1); 
    
    return 0;
}

int chromedriver_stop(void) {
    if (cd_pid <= 0) {
        fprintf(stderr, "No Chromedriver instance to stop\n");
        return -1;
    }
    if (kill(cd_pid, SIGTERM) != 0) {
        perror("kill");
        return -1;
    }
    waitpid(cd_pid, NULL, 0);
    cd_pid = -1;
    return 0;
}