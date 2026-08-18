typedef unsigned int u32;
typedef signed char s8;
typedef int s32;
typedef s32 Fix12i;

typedef struct { s32 x, y, z; } Vector3;
typedef struct { short x, y, z; } Vector3_16;

struct dActor_c {
    char pad1[0x98];
    Fix12i horzSpeed;
    char pad2[0x30];
    s8 areaID;
    char pad3[0x28f];
    Fix12i unk35c;
};

typedef struct dActor_c dActor_c;

extern dActor_c* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 actorID, u32 param1, const Vector3* pos, const Vector3_16* rot, s32 areaID, s32 deathTableID);

void _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
    dActor_c* self,
    const Vector3* pos,
    const Vector3_16* rot,
    Fix12i horzSpeed,
    Fix12i unk35c,
    u32 param1
) {
    dActor_c* a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xfe, param1, pos, rot, (s32)self->areaID, -1);
    if (a != (dActor_c*)0) {
        a->horzSpeed = horzSpeed;
        a->unk35c = unk35c;
    }
}
