#include "types.h"
typedef struct {
    u32 x : 12;
    u32 y : 12;
    u32 a : 1;
    u32 b : 2;
} TP;

typedef union {
    TP f;
    u16 h[2];
} TPU;

typedef struct {
    u16 x, y, a, b;
} Entry;

typedef struct {
    void (*fn)(int, int, int);
    Entry e4;
    u16 fc;
    char pad0e[2];
    Entry *f10;
    u16 f14;
    char pad16[0x32 - 0x16];
    u16 f32;
    volatile u16 f34;
    volatile u16 f36;
} S;
extern S data_020a80cc;

extern void Crash(void);

#pragma opt_strength_reduction off
void func_0205f300(int a0, unsigned int a1, int a2)
{
    u16 msg = (u16)a1;
    u32 bit = (u32)(msg & 0x7f00) << 8 >> 16;

    if (a2 != 0) {
        data_020a80cc.f34 |= 1 << bit;
        if (data_020a80cc.fn != 0)
            data_020a80cc.fn(bit, 4, 0);
        return;
    }

    if (bit == 0x10) {
        TPU t;
        Entry *e;
        u16 c;
        c = data_020a80cc.fc + 1;
        data_020a80cc.fc = c;
        if (data_020a80cc.fc >= data_020a80cc.f14)
            data_020a80cc.fc = 0;
        e = data_020a80cc.f10 + data_020a80cc.fc;
        t.h[0] = *(volatile u16 *)0x27fffaa;
        t.h[1] = *(volatile u16 *)0x27fffac;
        e->x = t.f.x;
        e->y = t.f.y;
        e->a = (unsigned char)t.f.a;
        e->b = (unsigned char)t.f.b;
        if (data_020a80cc.fn != 0)
            data_020a80cc.fn(bit, 0, data_020a80cc.fc & 0xff);
        return;
    }

    if ((a1 & 0x1000000) == 0)
        return;

    {
        int r4;
        switch (msg & 0xff) {
        case 0:
            switch (bit) {
            case 0: {
                TPU t;
                t.h[0] = *(volatile u16 *)0x27fffaa;
                t.h[1] = *(volatile u16 *)0x27fffac;
                data_020a80cc.f32 = 0;
                data_020a80cc.e4.x = t.f.x;
                data_020a80cc.e4.y = t.f.y;
                data_020a80cc.e4.a = (unsigned char)t.f.a;
                data_020a80cc.e4.b = (unsigned char)t.f.b;
                break;
            }
            case 1:
                data_020a80cc.f32 = 2;
                break;
            case 2:
                data_020a80cc.f32 = 0;
                break;
            }
            data_020a80cc.f36 &= ~(1 << bit);
            if (data_020a80cc.fn != 0)
                data_020a80cc.fn(bit, 0, 0);
            return;
        case 4:
            r4 = 3;
            break;
        case 2:
            r4 = 1;
            break;
        case 3:
            r4 = 2;
            break;
        default:
            goto crash;
        }
        data_020a80cc.f34 |= 1 << bit;
        data_020a80cc.f36 &= ~(1 << bit);
        if (data_020a80cc.fn != 0)
            data_020a80cc.fn(bit, r4 & 0xff, 0);
        return;
    }
crash:
    Crash();
}
