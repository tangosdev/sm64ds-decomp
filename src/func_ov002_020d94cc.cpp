//cpp
typedef unsigned char u8;
typedef unsigned int u32;
typedef short s16;
typedef signed char s8;

struct Vector3 { int x, y, z; };

extern "C" {
int func_ov002_020d94cc(char *self);
short GetAngleToCamera(int i);
void GiveVsStars(int idx, int delta);
void func_ov002_020e7218(char *a, char *b, int gate);
}

extern u8 data_0209f2d8;
extern s8 data_0209f310;

extern char *_ZN5Actor13SpawnSoundObjEj(char *self, u32 a);
extern char *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, const Vector3 &pos, const void *v, int e, int f);

extern "C" int func_ov002_020d94cc(char *self)
{
    Vector3 pos;
    char *sndobj;
    char *spawned;
    int ang, xx, zz;
    int xv, yv, sum, zv;
    int b;

    if ((&data_0209f310)[*(u8 *)(self + 0x6d8)] <= 0) goto fail;

    sndobj = _ZN5Actor13SpawnSoundObjEj(self, 3);
    b = (data_0209f2d8 == 1);
    if (b) {
        if (sndobj != 0) *(int *)(sndobj + 0xd4) = 0x50;
    }

    GiveVsStars(*(u8 *)(self + 0x6d8), -1);

    yv = *(int *)(self + 0x60);
    zv = *(int *)(self + 0x64);
    xv = *(int *)(self + 0x5c);
    sum = yv + 0x82000;
    pos.x = xv;
    pos.y = sum;
    pos.z = zv;

    b = (data_0209f2d8 == 1);
    if (b)
        spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb2, 0x10, pos, 0, *(s8 *)(self + 0xcc), -1);
    else
        spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb3, 0x10, pos, 0, *(s8 *)(self + 0xcc), -1);

    if (spawned == 0) goto fail;

    func_ov002_020e7218(spawned, self, 0);

    b = (data_0209f2d8 == 1);
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
    *(int *)(spawned + 0x98) = *(int *)(self + 0x98);
    return 1;
fail:
    return 0;
}
