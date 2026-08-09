typedef unsigned int u32;
typedef signed char s8;
typedef short s16;
typedef int s32;

typedef struct { s32 x, y, z; } Vector3;
typedef struct { short x, y, z; } Vector3_16;

struct Actor {
    char pad0[0xcc];
    s8 areaID;
    char pad1[0x100-0xcd];
    s16 f100;
    char pad2[0x3fc-0x102];
    u32 uniqueID;
    char pad3[0x408-0x400];
    s32 f408;
    char pad4[0x434-0x40c];
    Vector3 pos;
};
typedef struct Actor Actor;

extern Actor* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(u32 actorID, u32 param1, const Vector3* pos, const Vector3_16* rot, s32 areaID, s32 deathTableID);

int func_ov081_02124dfc(Actor* self)
{
    Actor* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
        0xe0, 0, &self->pos, (const Vector3_16*)0, (s32)self->areaID, -1);
    if (a)
        self->uniqueID = *(u32*)((char*)a + 4);
    *(s16*)((char*)self + 0x100) = 0x64;
    self->f408 = 0;
    return 1;
}
