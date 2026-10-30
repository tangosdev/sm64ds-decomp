/* NitroSDK SPI_tp.c TPi_TpCallback: the PXI tag 6 receive callback
 * TP_Init installs. Decodes the x:12/y:12/touch:1/validity:2 sample word,
 * updates the state and per-command error/busy bits, and calls the user
 * callback. */
#include "types.h"
#include "nitro/spi/tp.h"
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


extern void Crash(void);

#pragma opt_strength_reduction off
void TPi_TpCallback(int a0, unsigned int a1, int a2) {
    u16 msg = (u16)a1;
    u32 bit = (u32)(msg & 0x7f00) << 8 >> 16;

    if (a2 != 0) {
        data_020a80cc.errFlags |= 1 << bit;
        if (data_020a80cc.callback != 0)
            data_020a80cc.callback(bit, 4, 0);
        return;
    }

    if (bit == 0x10) {
        TPU t;
        TPData *e;
        u16 c;
        c = data_020a80cc.index + 1;
        data_020a80cc.index = c;
        if (data_020a80cc.index >= data_020a80cc.bufSize)
            data_020a80cc.index = 0;
        e = data_020a80cc.samplingBufs + data_020a80cc.index;
        t.h[0] = *(volatile u16 *)0x27fffaa;
        t.h[1] = *(volatile u16 *)0x27fffac;
        e->x = t.f.x;
        e->y = t.f.y;
        e->touch = (unsigned char)t.f.a;
        e->validity = (unsigned char)t.f.b;
        if (data_020a80cc.callback != 0)
            data_020a80cc.callback(bit, 0, data_020a80cc.index & 0xff);
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
                data_020a80cc.state = 0;
                data_020a80cc.buf.x = t.f.x;
                data_020a80cc.buf.y = t.f.y;
                data_020a80cc.buf.touch = (unsigned char)t.f.a;
                data_020a80cc.buf.validity = (unsigned char)t.f.b;
                break;
            }
            case 1:
                data_020a80cc.state = 2;
                break;
            case 2:
                data_020a80cc.state = 0;
                break;
            }
            data_020a80cc.commandFlags &= ~(1 << bit);
            if (data_020a80cc.callback != 0)
                data_020a80cc.callback(bit, 0, 0);
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
            goto crash; /* kept: default: Crash(); return; adds a branch (0x2c4) */
        }
        data_020a80cc.errFlags |= 1 << bit;
        data_020a80cc.commandFlags &= ~(1 << bit);
        if (data_020a80cc.callback != 0)
            data_020a80cc.callback(bit, r4 & 0xff, 0);
        return;
    }
crash:
    Crash();
}
