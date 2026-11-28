/* IS-NITRO debug console byte sinks and the formatter output callback.
 * func_0206ccd8 in cstd3.cpp installs func_0206cbc0 as the stream callback;
 * it selects the cartridge or main-memory ring from the debugger flags. */
#include "types.h"
#include "nitro/hw/mmap_shared.h"
#include "private/debugger_block.h"

#pragma optimize_for_size on

struct Ring {
    u16 head;
    u16 pad;
    volatile u16 tail;
};
struct Csl {
    int f0;
    int f4;
    int f8;
    int fc;
};

static inline int ReadByte(const void *a) {
    int p = (int)a;
    if (p & 1)
        return (*(u16 *)(p - 1) & 0xff00) >> 8;
    return *(u16 *)p & 0xff;
}

int func_0206cf7c(int mask);
void func_0206cec0(void *c);
void func_0206ce20(void *s);
void func_0206cd9c(void *s);
void func_0206ce90(u32 flags);
/* Byte sink for the cartridge-slot debug ring. */
void func_0206cb0c(void *obj, int val);
/* Byte sink for the main-memory debug ring. */
void func_0206ca7c(struct Ring *r, int val);
/* Output callback: send a formatted chunk to the selected debug ring. */
void func_0206cbc0(struct Csl *sl, u8 *sb, int count);

void func_0206cbc0(struct Csl *sl, u8 *sb, int count) {
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
        fn = (void (*)(void *, int))func_0206ca7c;
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

void func_0206cb0c(void *obj, int val) {
    /* Kept (#821 r79): not probed as a launder: the head is rewritten until it reads back,
       which needs a volatile access to mean anything. */
    volatile u16 *self = obj;
    u16 idx = self[0];
    u16 next = (idx + 1) & 0x7fff;
    unsigned char *bank;
    if (func_0206cf7c(0x2000)) {
        bank = (unsigned char *)0x09fd0000;
    } else {
        bank = (unsigned char *)0x08fd0000;
    }
    if (next != self[2]) {
        u16 *p = (u16 *)(bank + (idx & ~1));
        u16 cur = *p;
        u16 out;
        if ((idx & 1) != 0) {
            out = (cur & 0x00ff) | ((unsigned int)(val << 16) >> 8);
        } else {
            out = (cur & 0xff00) | ((unsigned int)(val << 16) >> 16);
        }
        *p = out;
        while (self[0] != next) {
            self[0] = next;
        }
    }
}

void func_0206ca7c(struct Ring *r, int val) {
    struct DebuggerBlock *sh = (struct DebuggerBlock *)HW_ISD_RESERVED;
    u16 head = r->head;
    u16 next = (head + 1) & 0x3fff;
    u16 tail = r->tail;
    char *buf = sh->buf + 0x8000;
    int off;
    u16 cur;
    u16 v;
    if (next == tail)
        return;
    off = head & ~1;
    cur = *(u16 *)(buf + off);
    if ((head & 1) != 0)
        v = (cur & 0xff) | ((u16)val << 8);
    else
        v = (cur & 0xff00) | (u16)val;
    *(u16 *)(buf + off) = v;
    r->head = next;
}
