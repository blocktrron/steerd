#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "aplist.h"
#include "mac.h"

static int bs_access_point_list_get_idx(struct bs_access_point_list *apl,
                                                            char *bssid)
{
    struct bs_access_point *ap;

    for (int i = 0; i < apl->len; i++) {
        ap = &apl->buf[i];
        if (mac_is_equal(ap->bssid, bssid))
            return i;
    }

    return -1;
}

static struct bs_access_point *__bs_access_point_list_get(struct bs_access_point_list *apl,
                                                          char *addr)
{
    int idx;

    idx = bs_access_point_list_get_idx(apl, addr);

    if (idx < 0)
        return NULL;

    return &apl->buf[idx];
}

static void bs_access_point_list_resize(struct bs_access_point_list *apl, size_t new_length)
{
    if (new_length > 0)
        apl->buf = realloc(apl->buf, new_length * sizeof(struct bs_access_point));
    else
        free(apl->buf);

    apl->len = new_length;
}

static struct bs_access_point *bs_access_point_list_add(struct bs_access_point_list *apl)
{
    struct bs_access_point *ap;

    bs_access_point_list_resize(apl, apl->len + 1);
    ap = &apl->buf[apl->len - 1];

    memset(ap, 0, sizeof(struct bs_access_point));

    return ap;
}

struct bs_access_point_list *bs_access_point_list_init()
{
    struct bs_access_point_list *apl;

    apl = malloc(sizeof(struct bs_access_point_list));
    memset(apl, 0, sizeof(struct bs_access_point_list));

    pthread_mutex_init(&apl->lock, NULL);

    return apl;
}

int bs_access_point_list_get_bssid(struct bs_access_point_list *apl,
                                   struct bs_access_point *buf,
                                   char *bssid)
{
    struct bs_access_point *ap;

    pthread_mutex_lock(&apl->lock);

    ap = __bs_access_point_list_get(apl, bssid);
    if (!ap)
        return 1;

    memcpy(buf, ap, sizeof(struct bs_access_point));

    pthread_mutex_unlock(&apl->lock);
    return 0;
}

void bs_access_point_list_update(struct bs_access_point_list *apl,
                                 char *bssid, char *ssid, char *ifname,
                                 int freq)
{
    struct bs_access_point *ap;

    if (!bssid)
        return;

    pthread_mutex_lock(&apl->lock);

    ap = __bs_access_point_list_get(apl, bssid);

    if (!ap)
        ap = bs_access_point_list_add(apl);

    memcpy(ap->bssid, bssid, MAC_LENGTH);
    if (ssid)
        memcpy(ap->ssid, ssid, 32);
    if (ifname)
        memcpy(ap->ifname, ifname, 16);
    if (freq != 0)
        ap->freq = freq;

    ap->time = time(0);

    pthread_mutex_unlock(&apl->lock);
}

int bs_access_point_list_remove(struct bs_access_point_list *apl, char *bssid)
{
    int last_idx;
    int idx;

    pthread_mutex_lock(&apl->lock);

    idx = bs_access_point_list_get_idx(apl, bssid);

    if (idx < 1)
        return 0;
    
    last_idx = apl->len - 1;

    if (idx != last_idx)
        memcpy(&apl->buf[idx], &apl->buf[last_idx], sizeof(struct bs_access_point));

    bs_access_point_list_resize(apl, apl->len - 1);

    pthread_mutex_unlock(&apl->lock);

    return 0;
}

void bs_access_point_list_free(struct bs_access_point_list *apl)
{
    pthread_mutex_destroy(&apl->lock);
    free(apl);
}