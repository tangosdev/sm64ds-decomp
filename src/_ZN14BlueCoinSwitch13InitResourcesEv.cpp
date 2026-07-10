//cpp
typedef int Fix12;
struct Vector3 { int x, y, z; };

struct MovingCylinderClsn {
    char dummy;
};

struct Actor {
    char pad8[8];
    unsigned int flags8;        // 0x8
    char pad_to8c[0x8c - 0xc];
    short f8c;                   // 0x8c
    short f8e;                   // 0x8e
    short f90;                   // 0x90
    char pad_to100[0x100 - 0x92];
    char f100[0x40];            // 0x100..
};

extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *clsn, struct Actor *a, Fix12 x, Fix12 z, unsigned int b, unsigned int c);
extern "C" void _ZN5Event8ClearBitEj(unsigned int bit);

extern "C" int _ZN14BlueCoinSwitch13InitResourcesEv(struct Actor *a)
{
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(
        (void *)((char *)a + 0xd4), a,
        ((a->f8c + 1) * 0x64) << 0xc,
        ((a->f8e + 1) * 0xc8) << 0xc,
        2, 0x400000);

    *(unsigned char *)((char *)a + 0x10d) = a->flags8 & 0x1f;
    *(unsigned char *)((char *)a + 0x10c) = (a->flags8 >> 5) & 1;

    {
        short t = a->f90;
        if (t <= 0)
            *(short *)((char *)a + 0x108) = 0x96;
        else
            *(short *)((char *)a + 0x108) = t;
    }
    *(short *)((char *)a + 0x10a) = 0;

    _ZN5Event8ClearBitEj(*(unsigned char *)((char *)a + 0x10d));
    return 1;
}
