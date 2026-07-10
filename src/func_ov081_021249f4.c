extern unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned a, unsigned b, int c, int d, int e, const void *f, void *g);
extern int ApproachAngle(short *angle, int target, int a, int b, int max);
extern void func_ov081_02125488(void *c, void *p);
extern char data_ov081_02128e84[];

struct Vec3 { int x, y, z; };

int func_ov081_021249f4(char *c)
{
    struct Vec3 pos;
    void *cb;
    int t;

    pos.x = *(int *)(c + 0x44c);
    pos.y = *(int *)(c + 0x450);
    pos.z = *(int *)(c + 0x454);
    cb = 0;
    *(unsigned *)(c + 0x460) =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned *)(c + 0x460), 0x11d, pos.x, pos.y, pos.z, 0, cb ? &pos : 0);
    pos.y = pos.y + 0x1e000;
    *(unsigned *)(c + 0x464) =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned *)(c + 0x464), 0x11e, pos.x, pos.y, pos.z, 0, cb ? &pos : 0);

    if (*(int *)(c + 0x450) > *(int *)(c + 0x60)) {
        char *b = c + 0x400;
        ApproachAngle((short *)(c + 0x8c), *(short *)(b + 0x14), 1, 0x1000, 0x1000);
    }
    t = *(int *)(c + 0x450) - 0x118000;
    if (t > *(int *)(c + 0x60)) {
        *(int *)(c + 0x60) = t;
        *(int *)(c + 0xa8) = 0;
        *(int *)(c + 0x9c) = 0;
        func_ov081_02125488(c, data_ov081_02128e84);
    }
    {
        short *ang = (short *)(((long long)(int)(c + 0x94)) & 0xFFFFFFFFFFFFFFFFLL);
        *ang = (short)(*ang + 0x2000);
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    }
    return 1;
}
