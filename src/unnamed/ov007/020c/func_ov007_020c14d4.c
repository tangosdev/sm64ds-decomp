#include "types.h"
extern u32 data_ov007_02104be8;
extern u32 data_ov007_02104be0;

extern void Crash(void);
extern void _ZN2GX16BeginLoadTexPlttEv(void);
extern void _ZN2GX11LoadTexPlttEPKvjj(const void *src, u32 slot, u32 size);
extern void _ZN2GX14EndLoadTexPlttEv(void);

struct Tex {
    u32 flags;
    char pad[0x18];
    const void *src;
    u32 size;
    u32 dest;
};

void func_ov007_020c14d4(struct Tex *t)
{
    u32 slot;
    u32 end;

    if (!(t->flags & 2)) return;

    slot = data_ov007_02104be8;
    end  = data_ov007_02104be0;

    if (slot + t->size > end)
        goto crash;

    _ZN2GX16BeginLoadTexPlttEv();
    _ZN2GX11LoadTexPlttEPKvjj(t->src, data_ov007_02104be8, t->size);
    _ZN2GX14EndLoadTexPlttEv();

    t->dest = data_ov007_02104be8;
    data_ov007_02104be8 += t->size;
    return;
crash:
    Crash();
}
