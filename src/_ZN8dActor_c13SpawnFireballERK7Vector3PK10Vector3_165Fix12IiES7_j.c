typedef unsigned int u32;
typedef signed char s8;
typedef int s32;
typedef s32 Fix12i;

typedef struct { s32 x, y, z; } Vector3;
typedef struct { short x, y, z; } Vector3_16;

struct Actor {
    char pad1[0x98];
    Fix12i horzSpeed;
    char pad2[0x30];
    s8 areaID;
    char pad3[0x28f];
    Fix12i unk35c;
};

typedef struct Actor Actor;

extern Actor* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(u32 actorID, u32 param1, const Vector3* pos, const Vector3_16* rot, s32 areaID, s32 deathTableID);

void _ZN5Actor13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
    Actor* self,
    const Vector3* pos,
    const Vector3_16* rot,
    Fix12i horzSpeed,
    Fix12i unk35c,
    u32 param1
) {
    Actor* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0xfe, param1, pos, rot, (s32)self->areaID, -1);
    if (a != (Actor*)0) {
        a->horzSpeed = horzSpeed;
        a->unk35c = unk35c;
    }
}
