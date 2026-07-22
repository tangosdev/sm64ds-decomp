struct Actor;
struct Vector3 { int x, y, z; };
struct Vector3_16;

typedef unsigned int u32;
typedef int s32;
typedef short s16;

extern struct Actor *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    u32 id, u32 param, const struct Vector3 *pos,
    const struct Vector3_16 *r, int a, int b);
extern int RandomIntInternal(int *seed);
extern int data_0209e650[];

void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(
    struct Actor *self, const struct Vector3 *pos, s32 count,
    s32 spread, s16 angle)
{
    struct Actor *actor;
    s32 ang;
    s32 prev;
    s32 i;
    u32 r;
    s16 ang0;
    struct Vector3_16 *rot;
    s32 deathId;
    u32 id;
    u32 param;

    ang0 = angle;
    rot = 0;
    id = 0x120;
    param = 2;
    deathId = -1;
    prev = 0xff;

    if (count > 1) {
        if (spread < 0x4000)
            spread = 0x4000;
    }

    for (i = 0; i < count; i++) {
        actor = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            id, param, pos, rot,
            *(signed char *)((char *)self + 0xcc), deathId);

        if (actor != 0) {
            do {
                ang = (s32)(((u32)RandomIntInternal(data_0209e650) >> 16) << 27) >> 16;
            } while (ang == prev);

            r = (u32)RandomIntInternal(data_0209e650);
            r >>= 16;

            *(s16 *)((char *)actor + 0x92) = 0;
            spread = (u32)((u32)spread * ((r % 50) + 100)) / 100;
            prev = ang;
            *(s16 *)((char *)actor + 0x94) = (s16)(ang0 + ang);
            *(s16 *)((char *)actor + 0x96) = 0;
            *(s32 *)((char *)actor + 0x98) = spread;
        }
    }
}
