//cpp
// @symbol _ZN11RollingRock8BehaviorEv
/* recovered: real C++ method
 *
 * MIGRATED 2026-08-27. The file's own note said this settled at THREE WORDS as a
 * method and that "spelling alone does not reach it" -- the ROM batches
 * `ldrh r2,[r4,#0x92]` and `ldrh r1,[r4,#0x94]` before the first store while the
 * C++ form reused r1 and interleaved. It reaches it; the lever is the one already
 * written down for C++ struct copies, applied at the copy rather than to the
 * destination type. See the comment on AngleWords below.
 *
 * The ten spellings the old note ruled out were all variations on WHERE the
 * scalarised copy happened. The fix is to stop it scalarising.
 */
#include "RollingRock.h"
#include "common.h"
/* The copy at 0x92 has to go through a struct whose only member is an ARRAY.
   In C `v16 = *(Vector3_16 *)(c + 0x92)` is a blind byte move; in C++ a struct
   copy scalarises to the members' own types, and Vector3_16's are s16, so the
   identical line becomes three LDRSH where the ROM has LDRH -- and mwcc then
   reuses one register, interleaving the store between the loads where the ROM
   batches both loads first. An array member restores both at once: unsigned
   halfword loads, and the ROM's load-load-store order. This was the last of the
   documented C++-vs-C struct-copy divergences to be measured on a real function;
   see notes/mwccarm-codegen.md. */
struct AngleWords { u16 w[3]; };
extern "C" {
extern void func_0200f760(char *c, void *p);
extern void func_ov021_021123b0(char *c);
extern void func_ov021_021127b4(char *c);
extern void func_ov021_02112544(char *c);
extern int func_ov021_021122fc(char *c);
extern void _ZN5dCc_c5ClearEv(void *thiz);
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void *thiz, const struct Vector3 *v);
extern void _ZN5dCc_c6UpdateEv(void *thiz);
extern void _ZN7fBase_c18MarkForDestructionEv(void *thiz);
extern void *_ZN8dActor_c10FindWithIDEj(u32 id);
extern void _ZN8dActor_c11UntrackStarERa(void *thiz, s8 *ref);
extern void _ZN8dActor_c14TriplePoofDustEv(void *thiz);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
extern void func_02012694(int a, void *p);
extern int _ZN8dActor_c13DistToCPlayerEv(void *thiz);
extern int RandomIntInternal(int *seed);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 a, u32 b, const struct Vector3 *pos, const struct Vector3_16 *ang, int e, int f);
extern int data_0209e650;

}

int RollingRock::Behavior()
{
    char *c = (char *)this;
    if (*(u8*)(c + 0x3be) >= 2) {
        func_0200f760(c, c + 0x1b8);
        func_ov021_021123b0(c);
        if (*(int*)(c + 0x10c) == 8) return 1;

        if (*(u8*)(c + 0x3be) != 4) {
            s16 *p8c;
            func_ov021_021127b4(c);
            p8c = (s16 *)(int)(c + 0x8c);
            *p8c = (s16)(*p8c + (*(int*)(c + 0x98) >> 12) * 0x43);
            *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
            func_ov021_02112544(c);
        }
        func_ov021_021122fc(c);
        _ZN5dCc_c5ClearEv(c + 0x1b8);

        if (*(int*)(c + 0x64) >= (int)0xfe82c000) {
            struct Vector3 v;
            v.x = 0;
            v.y = (int)0xffebb000;
            v.z = 0;
            _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(c + 0x1b8, &v);
        } else {
            struct Vector3 v;
            v.x = 0;
            v.y = -0xe1000;
            v.z = 0;
            _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(c + 0x1b8, &v);
        }
        _ZN5dCc_c6UpdateEv(c + 0x1b8);

        if (*(u8*)(c + 0x3be) != 4) {
            if (*(int*)(c + 0x60) < -0x3e8000) {
                _ZN7fBase_c18MarkForDestructionEv(c);
                if (*(u32*)(c + 0x3b4) != 0) {
                    char *a = (char*)_ZN8dActor_c10FindWithIDEj(*(u32*)(c + 0x3b4));
                    if (a) *(u8*)(a + 0x3c2) = 0;
                }
                _ZN8dActor_c11UntrackStarERa(c, (s8*)(c + 0x3c0));
                _ZN8dActor_c14TriplePoofDustEv(c);
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x67, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
                func_02012694(0x17a, c + 0x74);
            }
        }
    } else {
        int dist = _ZN8dActor_c13DistToCPlayerEv(c);
        if (dist > 0x5dc000) {
            u32 th = (dist < 0x1770000) ? 0x70 : 0xe0;
            if (*(u16*)(c + 0x100) >= th) {
                struct Vector3_16 v16;
                u32 rnd;
                u8 *p3bf;
                p3bf = (u8 *)(int)(c + 0x3bf);
                *p3bf = (u8)(*p3bf + 1);
                *(u16*)(c + 0x100) = 0;
                *(AngleWords *)&v16 = *(AngleWords *)(c + 0x92);
                rnd = (u32)RandomIntInternal(&data_0209e650);
                v16.y = v16.y + (rnd >> 16) % 0xc00;
                if (*(u8*)(c + 0x3be) == 1 && *(u8*)(c + 0x3bf) >= 5 && *(u8*)(c + 0x3c2) == 0) {
                    char *a;
                    *(u8*)(c + 0x3c2) = 1;
                    a = (char*)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                        0xdd, (*(u8*)(c + 0x3c1) << 8) | 3, (struct Vector3*)(c + 0x5c), &v16, *(s8*)(c + 0xcc), -1);
                    *(u8*)(c + 0x3bf) = 0;
                    if (a) *(u32*)(a + 0x3b4) = *(u32*)(c + 4);
                } else {
                    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                        0xdd, 2, (struct Vector3*)(c + 0x5c), &v16, *(s8*)(c + 0xcc), -1);
                }
            }
        }
        {
            u16 *p100 = (u16 *)(int)(c + 0x100);
            *p100 = (u16)(*p100 + 1);
        }
    }
    return 1;
}
