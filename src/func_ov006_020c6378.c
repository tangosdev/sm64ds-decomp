#include "types.h"
extern s16 data_02082214[];
extern void func_ov006_020c49d8(void* this);

void func_ov006_020c6378(int this)
{
    /* The two re-reads go through a const view of the object: without it
       mwcc 2004 CSEs the +0xea and +0xac field addresses into their own
       registers (ldrh ip,[ip] / add r1,r0,#0xac) and the function grows a
       word; the cartridge re-issues ldrh r2,[r0,#0xea] / ldr r1,[r0,#0xac]. */
    const char *ro = (const char*)this;
    *(s16*)(this + 0xea) += 0x200;
    *(int*)(this + 0xa8) = 0;
    *(int*)(this + 0xac) = data_02082214[(*(const u16*)(ro + 0xea) >> 4) * 2] >> 3;
    *(int*)(this + 0xa0) += *(const int*)(ro + 0xac);
    func_ov006_020c49d8((void*)this);
}
