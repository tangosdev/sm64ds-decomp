typedef unsigned int u32;
typedef signed char s8;
typedef int s32;

typedef struct { int x, y, z; } Vector3;
typedef struct { short x, y, z; } Vector3_16;

struct Actor {
    char pad[0xcc];
    s8 areaID;
};

typedef struct Actor Actor;

extern void _ZN5Actor11UntrackStarERa(Actor* self, s8* starID);
extern Actor* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 actorID, u32 param1, const Vector3* pos, const Vector3_16* rot, s32 areaID, s32 deathTableID);

typedef unsigned char u8;

Actor* _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j(Actor* self, s8* trackStarID, u32 starID, const Vector3* spawnPos, u8 howToSpawnStar) {
    _ZN5Actor11UntrackStarERa(self, trackStarID);
    return _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0xb2,
        starID | (howToSpawnStar << 4),
        spawnPos,
        (const Vector3_16*)0,
        (s32)self->areaID,
        -1
    );
}
