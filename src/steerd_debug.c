#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "steerd_debug.h"

int debug_level;

void steerd_set_level(int level)
{
    debug_level = level;
}

void steerd_printf(int level, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}
