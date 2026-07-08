//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };

extern "C" {
int func_ov002_020d93ac(char *self);
short GetAngleToCamera(int i);
}

extern u8 CURRENT_GAMEMODE;

struct Actor;
extern bool _ZN6Player8HasNoCapEv(char *self);
extern void _ZN6Player18SetNewHatCharacterEjjb(char *self, u32 a, u32 b, bool c);
extern char *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, const Vector3 &pos, const void *v, int e, int f);

extern "C" int func_ov002_020d93ac(char *self)
{
    Vector3 pos;
    char *spawned;
    u32 chr;
    int xv, yv, sum, zv;
    int b;
    int ang, xx, zz;

    if (_ZN6Player8HasNoCapEv(self)) goto fail;

    chr = *(u32 *)(self + 8) & 0xff;
    if (*(u8 *)(self + 0x6d9) == chr) goto fail;
    if (chr == 3) goto fail;

    _ZN6Player18SetNewHatCharacterEjjb(self, *(u8 *)(self + 0x6d9), 0, false);
    *(u8 *)(self + 0x71a) = 1;

    yv = *(int *)(self + 0x60);
    zv = *(int *)(self + 0x64);
    xv = *(int *)(self + 0x5c);
    sum = yv + 0x82000;
    pos.x = xv;
    pos.y = sum;
    pos.z = zv;
    spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x10d, (chr << 8) | 5,
        pos, 0, *(signed char *)(self + 0xcc), -1);
    if (spawned == 0) goto fail;

    b = (CURRENT_GAMEMODE == 1);
    if (!b) {
        ang = GetAngleToCamera(*(u8 *)(self + 0x6d8));
        zz = *(s16 *)(self + 0x96);
        xx = *(s16 *)(self + 0x92);
        ang += 0x8000;
        *(s16 *)(spawned + 0x92) = xx;
        *(s16 *)(spawned + 0x94) = ang;
        *(s16 *)(spawned + 0x96) = zz;
    } else {
        *(s16 *)(spawned + 0x92) = *(s16 *)(self + 0x92);
        *(s16 *)(spawned + 0x94) = *(s16 *)(self + 0x94);
        *(s16 *)(spawned + 0x96) = *(s16 *)(self + 0x96);
    }
    *(int *)(spawned + 0x98) = 0xb000;
    return 1;
fail:
    return 0;
}
