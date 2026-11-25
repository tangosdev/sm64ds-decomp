#include "types.h"
#include "nitro/hw/mmap_shared.h"
#include "private/debugger_block.h"
struct Csl {
    int f0;
    int f4;
    int f8;
    int fc;
};

int func_0206cf7c(int mask);
void func_0206cec0(void *c);
void func_0206ce20(void *s);
void func_0206cd9c(void *s);
void func_0206ce90(u32 flags);
void func_0206cb0c(void *c, int b);
void func_0206ca7c(void *c, int b);

/* The ISD debug console library was size-optimized. */
#pragma optimize_for_size on

/* One byte through the aligned u16 accesses this debug-output library uses. */
static inline int ReadByte(const void *a) {
    int p = (int)a;
    if (p & 1)
        return (*(u16 *)(p - 1) & 0xff00) >> 8;
    return *(u16 *)p & 0xff;
}

/* The output callback func_0206ccd8 gives func_0206a928: send count bytes
   to the debug console. It does nothing unless the debugger flags
   (func_0206cf7c) have 0x20000 and one sink: the cartridge ring
   (func_0206cb0c, flag 1) or the main-memory ring (func_0206ca7c, flag 4).
   It takes the cartridge lock (func_0206cec0) and sets the card bus
   (func_0206ce20) for the writes, then restores both. Bytes go out while
   the written count (+0xc) is below the limit (+4) minus one; the total
   (+8) counts them all. */
void func_0206cbc0(struct Csl *sl, u8 *sb, int count)
{
    struct DebuggerBlock *sh = (struct DebuggerBlock *)HW_ISD_RESERVED;
    void *base;
    void (*fn)(void *, int);
    int la[2];
    int lb[2];
    int v;

    base = sh->buf + 0x90;
    fn = 0;

    if (count == 0)
        return;
    if (func_0206cf7c(0x20000) == 0)
        return;
    if (func_0206cf7c(1) != 0) {
        fn = func_0206cb0c;
    } else if (func_0206cf7c(4) != 0) {
        fn = func_0206ca7c;
    }
    if (fn == 0)
        return;

    func_0206cec0(lb);
    func_0206ce20(la);

    if (sl != 0) {
        while (count-- != 0) {
            if ((u32)sl->fc < (u32)(sl->f4 - 1)) {
                v = ReadByte(sb);
                v = (u16)v;
                fn(base, v & 0xff);
                sl->fc += 1;
            }
            {
                int t = sl->f8;
                sb += 1;
                sl->f8 = t + 1;
            }
        }
    }

    func_0206cd9c(la);
    func_0206ce90(lb[0]);
}
