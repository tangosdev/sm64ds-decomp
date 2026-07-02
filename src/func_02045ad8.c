typedef unsigned int u32;

extern u32 gTexPlttCur;    /* 0x020a4bcc */
extern u32 gTexPlttEnd;    /* 0x020a4bd8 */

extern void Crash(void);
extern void _ZN2GX16BeginLoadTexPlttEv(void);
extern void _ZN2GX11LoadTexPlttEPKvjj(const void *src, u32 slot, u32 size);
extern void _ZN2GX14EndLoadTexPlttEv(void);

u32 func_02045ad8(const void *src, u32 size)
{
    u32 slot;
    u32 end;

    slot = gTexPlttCur;
    end  = gTexPlttEnd;

    if (slot + size > end)
        Crash();

    _ZN2GX16BeginLoadTexPlttEv();
    _ZN2GX11LoadTexPlttEPKvjj(src, gTexPlttCur, size);
    _ZN2GX14EndLoadTexPlttEv();

    slot = gTexPlttCur;
    gTexPlttCur = slot + ((size + 0xf) & 0xfff0);

    return slot;
}
