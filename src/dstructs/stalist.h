#ifndef __STEERD_STALIST_H
#define __STEERD_STALIST_H

#include <time.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#include "mac.h"


struct bs_beacon_report {
    char bssid[MAC_LENGTH];
    uint32_t rcpi;
    time_t time;
};

struct bs_station {
    char addr[MAC_LENGTH];
    char bssid[16];
    char ssid[32];
    int signal;
    time_t time;
    struct bs_beacon_report beacon_report;
};

struct bs_station_list {
    struct bs_station *buf;
    size_t len;
    pthread_mutex_t lock;
};

struct bs_station_list *bs_station_list_init();

int bs_station_list_get(struct bs_station_list *sl,
                                struct bs_station *buf,
                                char *addr);

int bs_station_list_update(struct bs_station_list *sl,
                                    char *addr, char *bssid, int signal,
                                    struct bs_beacon_report *beacon_report);

void bs_station_list_remove_stale(struct bs_station_list *sl, time_t max_age);

int bs_station_list_remove(struct bs_station_list *sl, char *addr);

void bs_station_list_free(struct bs_station_list *sl);

#endif