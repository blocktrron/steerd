#include <string.h>

#include "mac.h"

int mac_is_equal(char *mac0, char *mac1)
{
    return !memcmp(mac0, mac1, MAC_LENGTH);
}