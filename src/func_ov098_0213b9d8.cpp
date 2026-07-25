//cpp
// NONMATCHING: different op / idiom (div=46). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef short s16;
typedef int s32;
typedef long long s64;
typedef unsigned int u32;

struct Vector3 { s32 x, y, z; };
struct Vector3_16;
struct Actor;

extern "C" Actor *_ZN5Actor13ClosestPlayerEv(void);
extern "C" s32 Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern "C" Actor *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 actorID, u32 param1, const Vector3 *pos, const Vector3_16 *rot, s32 areaID, s32 deathTableID);
extern "C" void func_0201267c(u32 a, void *b, void *c, int d);
extern s16 data_02082214[];

extern "C" int func_ov098_0213b9d8(char *self)
{
    Vector3 v;
    Actor *player;
    int a;
    int factor;
    s16 sn, cs;

    if (*(unsigned short *)(self + 0x100)) return 0;
    player = _ZN5Actor13ClosestPlayerEv();
    if (!player) return 0;

    v.x = *(int *)((char *)player + 0x5c);
    v.y = *(int *)((char *)player + 0x60);
    v.z = *(int *)((char *)player + 0x64);
    if (Vec3_HorzDist(&v, (Vector3 *)(self + 0x5c)) > 0x5dc000) return 0;
    if (v.y > *(int *)(self + 0x60)) return 0;

    a = (unsigned short)*(s16 *)((char *)player + 0x94) >> 4;
    factor = (*(int *)((char *)player + 0x98) >> 0xc) * 0x1c + 0x64;
    cs = data_02082214[a * 2];
    sn = data_02082214[a * 2 + 1];
    v.y = v.y + 0x480000;
    v.x = factor * cs + v.x;
    v.z = factor * sn + v.z;

    {
        Actor *spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            0xd0, 1, &v, 0, *(signed char *)(self + 0xcc), -1);
        *(int *)((char *)spawned + 0x98) = 0;
    }
    *(unsigned short *)(self + 0x100) = 0x96;
    func_0201267c(0xd8, self + 0x74, 0, 0x96);
    return 1;
}
