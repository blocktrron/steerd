#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "stalist.h"
#include "mac.h"

static int bs_station_list_get_idx(struct bs_station_list *sl,
                                                  char *addr)
{
    struct bs_station *station;

    for (int i = 0; i < sl->len; i++) {
        station = &sl->buf[i];
        if (mac_is_equal(station->addr, addr))
            return i;
    }

    return -1;
}

static struct bs_station *__bs_station_list_get(struct bs_station_list *sl,
                                                  char *addr)
{
    int idx;

    idx = bs_station_list_get_idx(sl, addr);

    if (idx < 0)
        return NULL;

    return &sl->buf[idx];
}

static void bs_station_list_resize(struct bs_station_list *sl, size_t new_length)
{
    if (new_length > 0)
        sl->buf = realloc(sl->buf, new_length * sizeof(struct bs_station));
    else
        free(sl->buf);

    sl->len = new_length;
}

static struct bs_station *bs_station_list_add(struct bs_station_list *sl)
{
    bs_station_list_resize(sl, sl->len + 1);

    return &sl->buf[sl->len - 1];
}

struct bs_station_list *bs_station_list_init()
{
    struct bs_station_list *sl;

    sl = malloc(sizeof(struct bs_station_list));
    memset(sl, 0, sizeof(struct bs_station_list));

    pthread_mutex_init(&sl->lock, NULL);

    return sl;
}

int bs_station_list_get(struct bs_station_list *sl,
                                struct bs_station *buf,
                                char *addr)
{
    struct bs_station *sta;

    pthread_mutex_lock(&sl->lock);

    sta = __bs_station_list_get(sl, addr);
    if (!sta)
        return 1;

    memcpy(buf, sta, sizeof(struct bs_station));

    pthread_mutex_unlock(&sl->lock);
    return 0;
}

void bs_station_list_update(struct bs_station_list *sl,
                                    char *addr, char *bssid, int signal,
                                    struct bs_beacon_report *beacon_report)
{
    struct bs_station *station;

    if (!addr)
        return;

    pthread_mutex_lock(&sl->lock);

    station = __bs_station_list_get(sl, addr);

    if (!station)
        bs_station_list_add(sl);

    memcpy(station->addr, addr, MAC_LENGTH);
    if (bssid)
        memcpy(station->bssid, bssid, MAC_LENGTH);
    if (signal != 0)
        station->signal = signal;
    if (beacon_report)
        memcpy(&station->beacon_report, beacon_report, sizeof(struct bs_beacon_report));

    pthread_mutex_unlock(&sl->lock);
}

int bs_station_list_remove(struct bs_station_list *sl, char *addr)
{
    int last_idx;
    int idx;

    pthread_mutex_lock(&sl->lock);

    idx = bs_station_list_get_idx(sl, addr);

    if (idx < 1)
        return 0;
    
    last_idx = sl->len - 1;

    if (idx != last_idx)
        memcpy(&sl->buf[idx], &sl->buf[last_idx], sizeof(struct bs_station));

    bs_station_list_resize(sl, sl->len - 1);

    pthread_mutex_unlock(&sl->lock);

    return 0;
}

void bs_station_list_free(struct bs_station_list *sl)
{
    pthread_mutex_destroy(&sl->lock);
    free(sl);
}