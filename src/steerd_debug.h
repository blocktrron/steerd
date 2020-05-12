#ifndef __STEERD_DEBUG_H
#define __STEERD_DEBUG_H

enum {
    MSG_DEBUG,
    MSG_INFO,
    MSG_WARN,
    MSG_ERROR
};

void steerd_set_level(int level);
void steerd_printf(int level, const char *fmt, ...);

#endif