typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

typedef struct S {
    s32 mode;
    u32 flags;
    s32 a;
    s32 pad0c;
    s32 pad10;
    s32 b;
    s32 pad18;
    s32 cond;
    s32 c;
    s32 pad24;
    s32 f28;
    s32 f2c;
    union {
        s32 f30;
        unsigned char byte[4];
    };
} S;

extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void *p, int a, int b, int c, int d);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(void *p, int a, int b);

void func_ov007_020c86c4(S *p)
{
    p->a = 1;
    p->b = 0;
    p->c = 0;

    switch (p->mode) {
    case 0:
        break;

    case 1: {
        s32 mask = p->f28;
        if (p->flags & 1) {
            if (mask & 1) *(volatile u16 *)0x4000008 |= 0x40; else *(volatile u16 *)0x4000008 &= ~0x40;
            if (mask & 2) *(volatile u16 *)0x400000a |= 0x40; else *(volatile u16 *)0x400000a &= ~0x40;
            if (mask & 4) *(volatile u16 *)0x400000c |= 0x40; else *(volatile u16 *)0x400000c &= ~0x40;
            if (mask & 8) *(volatile u16 *)0x400000e |= 0x40; else *(volatile u16 *)0x400000e &= ~0x40;
        }
        if (!(p->flags & 2))
            return;
        if (mask & 1) *(volatile u16 *)0x4001008 |= 0x40; else *(volatile u16 *)0x4001008 &= ~0x40;
        if (mask & 2) *(volatile u16 *)0x400100a |= 0x40; else *(volatile u16 *)0x400100a &= ~0x40;
        if (mask & 4) *(volatile u16 *)0x400100c |= 0x40; else *(volatile u16 *)0x400100c &= ~0x40;
        if (mask & 8) *(volatile u16 *)0x400100e |= 0x40; else *(volatile u16 *)0x400100e &= ~0x40;
        return;
    }

    case 2:
    case 3:
        if (p->flags & 1) {
            _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4000050, p->f28, p->f2c,
                p->cond ? p->byte[2] : p->byte[0],
                p->cond ? p->byte[3] : p->byte[1]);
        }
        if (!(p->flags & 2))
            return;
        _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4001050, p->f28, p->f2c,
            p->cond ? p->byte[2] : p->byte[0],
            p->cond ? p->byte[3] : p->byte[1]);
        return;

    case 4:
        if (p->flags & 1) {
            _ZN3G2x18SetBlendBrightnessEPVtts((void *)0x4000050, p->f28,
                p->cond ? p->f30 : p->f2c);
        }
        if (!(p->flags & 2))
            return;
        _ZN3G2x18SetBlendBrightnessEPVtts((void *)0x4001050, p->f28,
            p->cond ? p->f30 : p->f2c);
        return;
    }
}
