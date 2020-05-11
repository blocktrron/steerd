#ifndef __STEERD_APLIST_H
#define __STEERD_APLIST_H

#include <time.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#include "mac.h"


struct bs_access_point {
    char bssid[MAC_LENGTH];
    char ssid[32];
    char ifname[16];
    int freq;
    time_t time;
};

struct bs_access_point_list {
    struct bs_access_point *buf;
    size_t len;
    pthread_mutex_t lock;
};

struct bs_access_point_list *bs_access_point_list_init();

int bs_access_point_list_get_bssid(struct bs_access_point_list *apl,
                                   struct bs_access_point *buf,
                                   char *bssid);

void bs_access_point_list_update(struct bs_access_point_list *apl,
                                 char *bssid, char *ssid, char *ifname,
                                 int freq);

int bs_access_point_list_remove(struct bs_access_point_list *apl, char *bssid);

void bs_access_point_list_free(struct bs_access_point_list *apl);

#endif