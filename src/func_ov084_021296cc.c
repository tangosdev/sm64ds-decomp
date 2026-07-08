// NONMATCHING: register allocation (div=16). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned int u32;

struct Vec3 { int x, y, z; };

extern void _ZN5Actor11UntrackStarERa(signed char *s);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 a, struct Vec3 *pos, void *rot, int e, int f);
extern void LinkSilverStarAndStarMarker(char *a, char *b);
extern void _ZN5Actor13SpawnSoundObjEj(void *c, u32 id);
extern u8 NUM_VS_STARS_COLLECTED[];
extern u8 *VS_STAR_SPAWN_ORDER;

void func_ov084_021296cc(char *c)
{
    if (*(u8*)(c + 0x464) == 1) {
        char *a, *b;
        _ZN5Actor11UntrackStarERa((signed char*)(c + 0x465));
        a = (char*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0xb4, 0x50, (struct Vec3*)(c + 0x41c), 0,
                *(signed char*)(c + 0xcc), -1);
        b = (char*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0xb3, 0x10, (struct Vec3*)(c + 0x5c), 0,
                *(signed char*)(c + 0xcc), -1);
        if (a != 0 && b != 0) {
            *(int*)(b + 0x434) = *(int*)(a + 4);
            LinkSilverStarAndStarMarker(a, b);
        }
        _ZN5Actor13SpawnSoundObjEj(c, 1);
        *(int*)(c + 8) &= 0xff0f;
        return;
    }
    if (*(u8*)(c + 0x464) != 2) return;
    if (*(u8*)(c + 0x466) != VS_STAR_SPAWN_ORDER[NUM_VS_STARS_COLLECTED[0]]) return;
    _ZN5Actor11UntrackStarERa((signed char*)(c + 0x465));
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            0xb4, *(u8*)(c + 0x466) | 0x30, (struct Vec3*)(c + 0x5c), 0,
            *(signed char*)(c + 0xcc), -1);
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            0xb3, *(u8*)(c + 0x466) | 0x30, (struct Vec3*)(c + 0x5c), 0,
            *(signed char*)(c + 0xcc), -1);
    *(u8*)(c + 0x464) = 3;
    *(int*)(c + 8) &= 0xff0f;
    _ZN5Actor13SpawnSoundObjEj(c, 1);
}
