extern void func_02012694(int a0, void *a1);
extern unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned a, unsigned b, int c, int d, int e, const void *f, void *g);

struct Vec3 { int x, y, z; };

int func_ov081_02124f7c(char *thiz)
{
    int n;
    struct Vec3 pos;
    void *cb;

    *(int *)(thiz + 0xa8) = 0x3c000;
    n = 0x4000;
    *(int *)(thiz + 0x9c) = -n;
    func_02012694(0xdc, thiz + 0x74);
    pos.x = *(int *)(thiz + 0x44c);
    pos.y = *(int *)(thiz + 0x450);
    pos.z = *(int *)(thiz + 0x454);
    cb = 0;
    *(unsigned *)(thiz + 0x460) =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned *)(thiz + 0x460), 0x11a, pos.x, pos.y, pos.z, 0,
            cb ? &pos : 0);
    pos.y = pos.y + 0x1e000;
    *(unsigned *)(thiz + 0x464) =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned *)(thiz + 0x464), 0x11b, pos.x, pos.y, pos.z, 0,
            cb ? &pos : 0);
    return 1;
}
