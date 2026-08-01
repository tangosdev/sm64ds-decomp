#include "types.h"
extern u32 data_ov007_02104bdc;
extern u32 data_ov007_02104be4;

extern void Crash(void);
extern void _ZN2GX12BeginLoadTexEv(void);
extern void _ZN2GX7LoadTexEPKvjj(const void *src, u32 slot, u32 size);
extern void _ZN2GX10EndLoadTexEv(void);

struct Tex {
    u32 flags;
    int pad4;
    const void *src;
    u32 size;
    u32 dest;
};

void func_ov007_020c1560(struct Tex *t)
{
    u32 slot;
    u32 end;

    if (!(t->flags & 1)) return;

    slot = data_ov007_02104bdc;
    end  = data_ov007_02104be4;

    if (slot + t->size > end)
        goto crash;

    _ZN2GX12BeginLoadTexEv();
    _ZN2GX7LoadTexEPKvjj(t->src, data_ov007_02104bdc, t->size);
    _ZN2GX10EndLoadTexEv();

    t->dest = data_ov007_02104bdc;
    data_ov007_02104bdc += t->size;
    return;
crash:
    Crash();
}
