// NONMATCHING: different op / idiom (div=45). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int s32;
typedef unsigned int u32;
typedef short s16;
struct Vector3 { s32 x, y, z; };
struct Vector3_16 { s16 x, y, z; };
extern s32 RNG_STATE[];
extern u32 RandomIntInternal(void *state);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 param, struct Vector3 *v, struct Vector3_16 *vr, s32 a, s32 b);
void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void *c, struct Vector3 *pos, u32 count, s32 spread, s16 angle){
    s32 i;
    s32 prev = 0xff;
    if (count > 1) {
        if (spread < 0x4000) spread = 0x4000;
    }
    for (i = 0; i < (s32)count; i++) {
        void *actor = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            0x120, 2, pos, (struct Vector3_16*)0,
            *(signed char*)((char*)c+0xcc), -1);
        if (actor != 0) {
            s32 ang;
            u32 r;
            do {
                ang = (s32)(((u32)RandomIntInternal(RNG_STATE) >> 16) << 27) >> 16;
            } while (ang == prev);
            r = (u32)RandomIntInternal(RNG_STATE) >> 16;
            *(s16*)((char*)actor+0x92) = 0;
            prev = ang;
            *(s32*)((char*)actor+0x98) = (u32)((u32)spread * ((r % 50) + 100)) / 100;
            *(s16*)((char*)actor+0x94) = (s16)(angle + ang);
            *(s16*)((char*)actor+0x96) = 0;
        }
    }
}
