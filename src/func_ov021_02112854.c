// NONMATCHING: extra logic (you do more) (div=47). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef short s16;

struct Vector3 { int x, y, z; };
struct Vector3_16 { s16 x, y, z; };

extern void func_0200f760(char *c, void *p);
extern void func_ov021_021123b0(char *c);
extern void func_ov021_021127b4(char *c);
extern void func_ov021_02112544(char *c);
extern int func_ov021_021122fc(char *c);
extern void _ZN12CylinderClsn5ClearEv(void *thiz);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *thiz, const struct Vector3 *v);
extern void _ZN12CylinderClsn6UpdateEv(void *thiz);
extern void _ZN9ActorBase18MarkForDestructionEv(void *thiz);
extern void *_ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN5Actor11UntrackStarERa(void *thiz, s8 *ref);
extern void _ZN5Actor14TriplePoofDustEv(void *thiz);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
extern void func_02012694(int a, void *p);
extern int _ZN5Actor13DistToCPlayerEv(void *thiz);
extern int RandomIntInternal(int *seed);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, const struct Vector3 *pos, const struct Vector3_16 *ang, int e, int f);
extern int RNG_STATE;

int func_ov021_02112854(char *c)
{
    if (*(u8*)(c + 0x3be) >= 2) {
        func_0200f760(c, c + 0x1b8);
        func_ov021_021123b0(c);
        if (*(int*)(c + 0x10c) == 8) return 1;

        if (*(u8*)(c + 0x3be) != 4) {
            func_ov021_021127b4(c);
            *(s16*)(c + 0x8c) = *(s16*)(c + 0x8c) + (*(int*)(c + 0x98) >> 12) * 0x43;
            *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
            func_ov021_02112544(c);
        }
        func_ov021_021122fc(c);
        _ZN12CylinderClsn5ClearEv(c + 0x1b8);

        if (*(int*)(c + 0x64) >= (int)0xfe82c000) {
            struct Vector3 v;
            v.x = 0;
            v.y = (int)0xffebb000;
            v.z = 0;
            _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x1b8, &v);
        } else {
            struct Vector3 v;
            v.x = 0;
            v.y = -0xe1000;
            v.z = 0;
            _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x1b8, &v);
        }
        _ZN12CylinderClsn6UpdateEv(c + 0x1b8);

        if (*(u8*)(c + 0x3be) != 4) {
            if (*(int*)(c + 0x60) < -0x3e8000) {
                _ZN9ActorBase18MarkForDestructionEv(c);
                if (*(u32*)(c + 0x3b4) != 0) {
                    char *a = (char*)_ZN5Actor10FindWithIDEj(*(u32*)(c + 0x3b4));
                    if (a) *(u8*)(a + 0x3c2) = 0;
                }
                _ZN5Actor11UntrackStarERa(c, (s8*)(c + 0x3c0));
                _ZN5Actor14TriplePoofDustEv(c);
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x67, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
                func_02012694(0x17a, c + 0x74);
            }
        }
        return 1;
    }

    {
        int dist = _ZN5Actor13DistToCPlayerEv(c);
        if (dist > 0x5dc000) {
            int th = (dist < 0x1770000) ? 0x70 : 0xe0;
            if (*(u16*)(c + 0x100) >= th) {
                struct Vector3_16 v16;
                u32 rnd;
                (*(u8*)(c + 0x3bf))++;
                *(u16*)(c + 0x100) = 0;
                v16.x = *(u16*)(c + 0x92);
                v16.y = *(u16*)(c + 0x94);
                v16.z = *(u16*)(c + 0x96);
                rnd = (u32)RandomIntInternal(&RNG_STATE);
                v16.y = v16.y + (rnd >> 16) % 0xc00;
                if (*(u8*)(c + 0x3be) == 1 && *(u8*)(c + 0x3bf) >= 5 && *(u8*)(c + 0x3c2) == 0) {
                    char *a;
                    *(u8*)(c + 0x3c2) = 1;
                    a = (char*)_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                        0xdd, (*(u8*)(c + 0x3c1) << 8) | 3, (struct Vector3*)(c + 0x5c), &v16, *(s8*)(c + 0xcc), -1);
                    *(u8*)(c + 0x3bf) = 0;
                    if (a) *(u32*)(a + 0x3b4) = *(u32*)(c + 4);
                } else {
                    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                        0xdd, 2, (struct Vector3*)(c + 0x5c), &v16, *(s8*)(c + 0xcc), -1);
                }
            }
        }
        (*(u16*)(c + 0x100))++;
    }
    return 1;
}
