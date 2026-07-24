typedef struct { int x, y, z; } Vector3;

extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int a, unsigned int b, int fix, int t1, int t2, void *v, void *cb);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, void *v, unsigned int d);

#define M(p) ((long long)(int)(p) & 0xFFFFFFFFFFFFFFFFLL)

int func_ov060_021132a4(char *c)
{
    int r4 = 0;
    volatile Vector3 pos;
    int ytmp;
    int z;

    pos.x = *(int *)(c + 0x5c);
    ytmp = *(int *)(c + 0x60);
    pos.y = ytmp;
    z = *(int *)(c + 0x64);
    pos.z = z;
    pos.y = ytmp + 0x32000;

    *(int *)(c + 0x448) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(volatile int *)(c + 0x448), 0x99, pos.x, pos.y, z, 0, 0);

    if (*(int *)(c + 0x80) < 0xccc) {
        short *p402 = (short *)(int)M(c + 0x402);
        *p402 = (short)(*p402 + 0x80);
    }

    if (*(int *)(c + 0x80) > 0x334) {
        int *p80 = (int *)(int)M(c + 0x80);
        int *p88 = (int *)(int)M(c + 0x88);
        *p80 = *p80 - 0x52;
        *p88 = *p88 - 0x52;
    } else {
        int *p84 = (int *)(int)M(c + 0x84);
        *p84 = *p84 - 0x29;
        *(int *)(c + 0xa8) = 0xa000;
        *(int *)(c + 0x9c) = 0;
    }

    if (*(int *)(c + 0x84) < 0x800)
        r4 = 1;

    {
        short *p8e = (short *)(int)M(c + 0x8e);
        short *p402b = (short *)(int)M(c + 0x400);
        *p8e = (short)(*p8e + p402b[1]);
    }

    if (*(unsigned char *)(c + 0x41c) > 2) {
        unsigned char *p41c = (unsigned char *)(int)M(c + 0x41c);
        *p41c = (unsigned char)(*p41c - 2);
    }

    if (r4 == 0) {
        if (*(int *)(c + 0x450) != 0xba)
            *(int *)(c + 0x44c) = 0;
        *(int *)(c + 0x450) = 0xba;
        *(int *)(c + 0x44c) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x44c), 3, *(int *)(c + 0x450), c + 0x74, 0);
    }
    return r4;
}
