#include "types.h"
struct S {
    s32 mode;
    u32 flags;
    s32 a;
    s32 pad[2];
    s32 b;
    s32 pad2[2];
    s32 c;
};

void func_ov007_020c897c(struct S *p) {
    p->a = 0;
    p->b = 0x1000;
    p->c = 0;
    switch (p->mode) {
    case 1:
        if (p->flags & 1) {
            *(volatile u16 *)0x4000008 &= ~0x40;
            *(volatile u16 *)0x400000a &= ~0x40;
            *(volatile u16 *)0x400000c &= ~0x40;
            *(volatile u16 *)0x400000e &= ~0x40;
        }
        if (p->flags & 2) {
            *(volatile u16 *)0x4001008 &= ~0x40;
            *(volatile u16 *)0x400100a &= ~0x40;
            *(volatile u16 *)0x400100c &= ~0x40;
            *(volatile u16 *)0x400100e &= ~0x40;
        }
        break;
    case 2:
    case 3:
    case 4:
        if (p->flags & 1) {
            *(volatile u16 *)0x4000050 = 0;
        }
        if (p->flags & 2) {
            *(volatile u16 *)0x4001050 = 0;
        }
        break;
    }
}
