typedef unsigned int u32;
typedef int s32;

typedef struct { int x, y, z; } Vector3;
typedef struct { short x, y, z; } Vector3_16;

struct Actor {
    u32 vtable;     /* 0x0 */
    u32 uniqueID;   /* 0x4 */
};

typedef struct Actor Actor;

extern Actor* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 actorID, u32 param1, const Vector3* pos, const Vector3_16* rot, s32 areaID, s32 deathTableID);

extern u32 data_0209b2a4[0x10]; /* 0x0209b2a4 */

Actor* func_0200e4e4(u32 param1)
{
    u32* slot = data_0209b2a4;
    s32 i = 0;
    for (;;) {
        if (*slot == 0) {
            Actor* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x160, param1, 0, 0, -1, -1);
            if (a)
                *slot = a->uniqueID;
            return a;
        }
        i++;
        slot++;
        if (i >= 0x10)
            break;
    }
    return 0;
}