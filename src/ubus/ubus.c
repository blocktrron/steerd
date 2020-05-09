#include <libubus.h>

#include "aplist.h"
#include "stalist.h"

static struct ubus_context *ctx;

struct bs_station_list *station_list;
struct bs_access_point_list *ap_list;

void update_iwinfo(struct uloop_timeout *t);

struct uloop_timeout iwinfo_timer = {
        .cb = update_iwinfo
};

void update_iwinfo(struct uloop_timeout *t) {
    printf("Update iwinfo\n");
    uloop_timeout_set(&iwinfo_timer, 10 * 1000);
}

void update_clients(struct uloop_timeout *t);

struct uloop_timeout client_timer = {
        .cb = update_clients
};

void update_clients(struct uloop_timeout *t) {
    printf("Update clients\n");
    uloop_timeout_set(&client_timer, 10 * 1000);
}

void update_beacon_reports(struct uloop_timeout *t);

struct uloop_timeout beacon_reports_timer = {
        .cb = update_beacon_reports
};

void update_beacon_reports(struct uloop_timeout *t) {
    printf("Update beacon reports\n");
    uloop_timeout_set(&client_timer, 10 * 1000);
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