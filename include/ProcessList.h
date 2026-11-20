#ifndef PROCESSLIST_H
#define PROCESSLIST_H

#include "fBase_c.h"

/* The five frame-loop lists. The heads live in fListNode.cpp.
 * The callback is the 8-byte member pointer fBase_c::ProcessFunction;
 * the four list heads are 0x10 bytes each. */
#ifndef SM64DS_PLATFORM_PC
typedef char
    fBase_c_ProcessFunction_size_must_be_0x8[sizeof(fBase_c::ProcessFunction) == 0x8 ? 1 : -1];
#endif

struct ProcessList {
    fLiNdBaPr_c *head;
    fLiNdBaPr_c *tail;
    fBase_c::ProcessFunction callback;

    ProcessList(fBase_c::ProcessFunction &p) {
        head = 0;
        tail = 0;
        callback = p;
    }

    static int func_020441bc(int);
};

#endif
