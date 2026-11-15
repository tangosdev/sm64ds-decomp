#ifndef PROCESSLIST_H
#define PROCESSLIST_H

#include "fBase_c.h"

/* The five frame-loop lists. The heads live in fListNode.cpp. */
struct ProcessList {
    fLiNdBaPr_c *head;
    fLiNdBaPr_c *tail;
    fBase_c::ProcessFunction callback;

    ProcessList(fBase_c::ProcessFunction &p) {
        head = 0;
        tail = 0;
        callback = p;
    }

    static void func_02044120();
    static int func_020441bc(int);
};

#endif
