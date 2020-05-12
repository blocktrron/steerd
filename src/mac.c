#include <string.h>
#include <stdlib.h>

#include "mac.h"

int mac_is_equal(char *mac0, char *mac1)
{
    return !memcmp(mac0, mac1, MAC_LENGTH);
}

void macstrtol(char *out, char *in)
{
    out[0] = strtol(&in[0],  NULL, 16);
    out[1] = strtol(&in[3],  NULL, 16);
    out[2] = strtol(&in[6],  NULL, 16);
    out[3] = strtol(&in[9],  NULL, 16);
    out[4] = strtol(&in[12], NULL, 16);
    out[5] = strtol(&in[15], NULL, 16);
}