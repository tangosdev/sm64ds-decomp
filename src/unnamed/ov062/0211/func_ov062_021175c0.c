typedef unsigned int u32;
typedef signed int s32;
typedef long long s64;

typedef struct { int x, y, z; } Vector3;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 uniqueID, u32 effectID, s32 x, s32 y, s32 z, const void *dir, void *callback);
extern void _ZN8Particle19SetSelfDestructFlagEj(u32 id);
extern void *_ZN8Particle6System12FromUniqueIDEj(u32 id);

void func_ov062_021175c0(char *c)
{
    volatile Vector3 pos;
    int t;
    u32 spawned;
    void *particle;

    if (*(unsigned char *)(c + 0x3ce) != 0) return;

    pos.x = *(int *)(c + 0x5c);
    pos.y = *(int *)(c + 0x60);
    pos.z = *(int *)(c + 0x64);

    t = *(unsigned short *)(c + 0xc);
    t = t == 0xcb;
    {
        int zArg = pos.z;
        if (t != 0) {
            pos.y = pos.y + 0xf000;
        } else {
            pos.y = pos.y + 0xa000;
        }

        spawned = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 0xb2, pos.x, pos.y, zArg, (void *)0, (void *)0);
    }

    _ZN8Particle19SetSelfDestructFlagEj(0xb2);

    *(unsigned char *)(c + 0x3ce) = 0xa;
    if (spawned == 0) return;

    particle = _ZN8Particle6System12FromUniqueIDEj(spawned);
    if (particle == 0) return;

    {
        int t2 = *(unsigned short *)(c + 0xc);
        t2 = t2 == 0xcb;
        if (t2 != 0) {
            int v = *(int *)((char *)particle + 0x50);
            int r = (int)((((s64)v) * 0x800 + 0x800) >> 12);
            *(int *)((char *)particle + 0x50) = (short)r;
        } else {
            int v = *(int *)((char *)particle + 0x50);
            int r = (int)((((s64)v) * 0x500 + 0x800) >> 12);
            *(int *)((char *)particle + 0x50) = (short)r;
        }
    }
}
