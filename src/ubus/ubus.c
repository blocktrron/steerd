#include <glob.h>
#include <iwinfo.h>
#include <libubus.h>
#include <stdlib.h>

#include "aplist.h"
#include "stalist.h"
#include "mac.h"
#include "steerd_debug.h"

static struct ubus_context *ctx;

struct bs_station_list *station_list;
struct bs_access_point_list *ap_list;

void update_iwinfo(struct uloop_timeout *t);

static void update_ap_from_iwinfo(struct bs_access_point_list *apl,
                                  char *ifname)
{
    char ssid[IWINFO_ESSID_MAX_SIZE + 1];
    const struct iwinfo_ops *iw;
    char bssidstr[18];
    char bssid[6];
    int frequency;
    int channel;

    iw = iwinfo_backend(ifname);

    if (!iw)
        return;

    iw->bssid(ifname, bssidstr);
    iw->ssid(ifname, ssid);
    iw->channel(ifname, &channel);
    iw->frequency(ifname, &frequency);

    macstrtol(bssid, bssidstr);

    steerd_printf(MSG_DEBUG, "Updating AP %s (%s)", ifname, bssidstr);

    bs_access_point_list_update(apl, bssid, ssid, ifname, frequency);
    iwinfo_finish();
}

static void update_clients_for_interface(struct bs_station_list *sl,
                                         char *ifname)
{
    struct iwinfo_assoclist_entry *entry;
    const struct iwinfo_ops *iw;
    char buf[IWINFO_BUFSIZE];
    char bssidstr[18];
    char bssid[8];
    int new_sta;
    int len;

    iw = iwinfo_backend(ifname);

    if (!iw)
        return;

    iw->assoclist(ifname, buf, &len);
    iw->bssid(ifname, bssidstr);

    macstrtol(bssid, bssidstr);
    
    for (int i = 0; i < len; i += sizeof(struct iwinfo_assoclist_entry)) {
        entry = (struct iwinfo_assoclist_entry *) &buf[i];
        new_sta = bs_station_list_update(sl, bssid, (char *)entry->mac, entry->signal, NULL);
        if (new_sta > 0)
            steerd_printf(MSG_DEBUG, "Adding STA for %s (%s)", ifname, bssidstr);
    }

    iwinfo_finish();
}

struct uloop_timeout iwinfo_timer = {
        .cb = update_iwinfo
};

void update_iwinfo(struct uloop_timeout *t)
{
    glob_t globbuf;
    char *ifname;

    steerd_printf(MSG_DEBUG, "Updating APs");

    glob("/sys/class/net/*", 0, NULL, &globbuf);

    for (int i = 0; i < globbuf.gl_pathc; i++) {
        ifname = strrchr(globbuf.gl_pathv[i], '/');

        if (!ifname)
            continue;

        update_ap_from_iwinfo(ap_list, ++ifname);
    }

    globfree(&globbuf);
    uloop_timeout_set(&iwinfo_timer, 60 * 1000);
}

void update_clients(struct uloop_timeout *t);

struct uloop_timeout client_timer = {
        .cb = update_clients
};

void update_clients(struct uloop_timeout *t) {
    glob_t globbuf;
    char *ifname;

    steerd_printf(MSG_DEBUG, "Updating clients");

    glob("/sys/class/net/*", 0, NULL, &globbuf);

    for (int i = 0; i < globbuf.gl_pathc; i++) {
        ifname = strrchr(globbuf.gl_pathv[i], '/');

        if (!ifname)
            continue;

        update_clients_for_interface(station_list, ++ifname);
    }

    globfree(&globbuf);
    uloop_timeout_set(&client_timer, 10 * 1000);
}

void update_beacon_reports(struct uloop_timeout *t);

struct uloop_timeout beacon_reports_timer = {
        .cb = update_beacon_reports
};

void update_beacon_reports(struct uloop_timeout *t) {
    steerd_printf(MSG_DEBUG, "Updating beacon reports");
    uloop_timeout_set(&beacon_reports_timer, 10 * 1000);
}

int init_buffers()
{
    station_list = bs_station_list_init();
    ap_list = bs_access_point_list_init();
    return 0;
}

int free_buffers()
{
    bs_station_list_free(station_list);
    bs_access_point_list_free(ap_list);
    return 0;
}

int init_ubus(const char *ubus_socket)
{
    uloop_init();

    ctx = ubus_connect(ubus_socket);
    if (!ctx)
        return 1;

    init_buffers();

    ubus_add_uloop(ctx);

    uloop_timeout_add(&iwinfo_timer);
    uloop_timeout_add(&client_timer);
    uloop_timeout_add(&beacon_reports_timer);

    uloop_run();

    ubus_free(ctx);
    uloop_done();
    free_buffers();
    return 0;
}