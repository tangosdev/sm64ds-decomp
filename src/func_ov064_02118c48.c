// Spawns actor 0x120 (param 2) at this actor's position with the signed byte
// param at +0xcc; if this actor stores a link ID at +0x320, finds that actor,
// copies its unique ID (+4) into the spawned actor's +0xd4, and bumps the
// found actor's byte refcount at +0xd6.
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, const struct Vector3* pos, const struct Vector3_16* rot, int e, int f);
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);

void func_ov064_02118c48(char* r5)
{
    char* spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x120, 2, (struct Vector3*)(r5 + 0x5c), 0, *(signed char*)(r5 + 0xcc), -1);
    char* found;
    if (spawned == 0)
        return;
    if (*(int*)(r5 + 0x320) == 0)
        return;
    found = _ZN5Actor10FindWithIDEj(*(int*)(r5 + 0x320));
    if (found == 0)
        return;
    *(int*)(spawned + 0xd4) = *(int*)(found + 4);
    (*(unsigned char*)((long long)(int)(found + 0xd6) & 0xFFFFFFFFFFFFFFFFLL)) += 1;
}
