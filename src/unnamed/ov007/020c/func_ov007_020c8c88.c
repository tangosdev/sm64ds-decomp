typedef long long s64;

struct Obj {
    int p0;
    unsigned int p4;
    char pad[0x28 - 8];
    unsigned int p28;
    unsigned char p2c;
    unsigned char p2d;
};

void func_ov007_020c8c88(struct Obj *o, int t)
{
    unsigned char from = o->p2c;
    unsigned char to = o->p2d;
    int loFrom = from & 0xf;
    unsigned int flags = o->p28;
    int hiFrom = (int)(from & 0xf0) >> 4;
    int loTo = to & 0xf;
    int hiTo = (int)(to & 0xf0) >> 4;
    int lo, hi;

    if (t >= 0x1000) {
        lo = loTo;
    } else if (t <= 0) {
        lo = loFrom;
    } else {
        lo = (int)((((s64)(0x1000 - t) * (loFrom << 12) +
                     (s64)t * (loTo << 12)) >> 12)) >> 12;
    }

    if (t >= 0x1000) {
        hi = hiTo;
    } else if (t <= 0) {
        hi = hiFrom;
    } else {
        hi = (int)((((s64)(0x1000 - t) * (hiFrom << 12) +
                     (s64)t * (hiTo << 12)) >> 12)) >> 12;
    }

    if (o->p4 & 1) {
        (void)*(volatile unsigned short *)0x400004c;
        if ((flags & 1) || (flags & 2) || (flags & 4) || (flags & 8))
            *(volatile unsigned char *)0x400004c =
                (unsigned char)(lo | (hi << 4));
        if (flags & 0x10)
            *(volatile unsigned char *)0x400004d =
                (unsigned char)(lo | (hi << 4));
    }

    if (!(o->p4 & 2))
        return;

    if (flags & 0x10)
        *(volatile unsigned char *)0x400104d =
            (unsigned char)(lo | (hi << 4));

    if ((flags & 1) || (flags & 2) || (flags & 4) || (flags & 8))
        *(volatile unsigned char *)0x400104c =
            (unsigned char)(lo | (hi << 4));
}
