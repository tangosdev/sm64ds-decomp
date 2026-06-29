typedef unsigned short u16;

extern void MultiStore_Int(int val, int *dst, int len);
extern void MultiStore16(u16 val, char *dst, int nbytes);

struct S {
    int mode;       // 0x0
    int flags;      // 0x4
    short pad8;     // 0x8
    u16 count;      // 0xa
    char gap[0x18-0xc];
    int *p18;       // 0x18
    int *p1c;       // 0x1c
    int *p20;       // 0x20
    int *p24;       // 0x24
    int *p28;       // 0x28
    int *p2c;       // 0x2c
};

void func_ov007_020c28ac(struct S *c)
{
    c->pad8 = 0;
    switch (c->mode) {
    case 0: {
        volatile int a = 0;
        MultiStore_Int(a, c->p24, c->count << 2);
        {
            volatile int b = 0;
            MultiStore_Int(b, c->p28, c->count << 2);
        }
        break;
    }
    case 1: {
        volatile int a = 0;
        MultiStore_Int(a, c->p2c, c->count * 0xc);
        break;
    }
    }
    switch (c->mode) {
    case 0: {
        volatile u16 a = 0;
        MultiStore16(a, (char *)c->p1c, c->count << 1);
    }
    case 1:
    default: {
        volatile int b = 0;
        MultiStore_Int(b, c->p18, c->count << 2);
    }
    }
    if (c->flags & 1) {
        volatile int a = 0;
        MultiStore_Int(a, c->p20, c->count << 2);
    }
}
