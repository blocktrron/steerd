#include <libubus.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "ubus.h"

struct sigaction signal_action;

void daemon_shutdown() {
    uloop_cancelled = true;
}

void signal_handler(int sig) {
    switch (sig) {
        case SIGHUP:
            daemon_shutdown();
            break;
        case SIGINT:
            daemon_shutdown();
            break;
        case SIGTERM:
            daemon_shutdown();
            exit(EXIT_SUCCESS);
        default:
            daemon_shutdown();
            break;
    }
}

int main(int argc, char **argv) {
    const char *ubus_socket = NULL;

    argc -= optind;
    argv += optind;

    // connect signals
    signal_action.sa_handler = signal_handler;
    sigemptyset(&signal_action.sa_mask);
    signal_action.sa_flags = 0;
    sigaction(SIGHUP, &signal_action, NULL);
    sigaction(SIGTERM, &signal_action, NULL);
    sigaction(SIGINT, &signal_action, NULL);

    init_ubus(ubus_socket);

    return 0;
}