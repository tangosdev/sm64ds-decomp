#include "types.h"
extern u32 data_020a4bcc;    /* 0x020a4bcc */
extern u32 data_020a4bd8;    /* 0x020a4bd8 */

extern void Crash(void);
extern void _ZN2GX16BeginLoadTexPlttEv(void);
extern void _ZN2GX11LoadTexPlttEPKvjj(const void *src, u32 slot, u32 size);
extern void _ZN2GX14EndLoadTexPlttEv(void);

u32 func_02045ad8(const void *src, u32 size)
{
    u32 slot;
    u32 end;

    slot = data_020a4bcc;
    end  = data_020a4bd8;

    if (slot + size > end)
        Crash();

    _ZN2GX16BeginLoadTexPlttEv();
    _ZN2GX11LoadTexPlttEPKvjj(src, data_020a4bcc, size);
    _ZN2GX14EndLoadTexPlttEv();

    slot = data_020a4bcc;
    data_020a4bcc = slot + ((size + 0xf) & 0xfff0);

    return slot;
}
